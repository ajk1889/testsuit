#include <thread>
#include "StreamDescriptor.h"
#include "../../server/ServerParams.h"

ssize_t StreamDescriptor::read(char *buffer, const uint N) {
    if (N && unreadBytesCount) {
        auto availableBytesLen = min(N, unreadBytesCount);
        unreadBytesCount -= availableBytesLen;
        memcpy(buffer, unreadBytes, availableBytesLen);
        memmove(const_cast<char *>(unreadBytes),
                unreadBytes + availableBytesLen, unreadBytesCount);
        return availableBytesLen;
    } else if (N) {
        auto bytesToRead = min(max(1, params.readBytesPerTimeDiff - thisSectionReadCount), N);
        auto n = ::read(descriptor, buffer, bytesToRead);
        thisSectionReadCount += n;
        auto now = preciseNow();
        if (thisSectionReadCount >= params.readBytesPerTimeDiff || now >= thisReadTimeSectionEndTime) {
            std::this_thread::sleep_for(thisReadTimeSectionEndTime - now);
            if (thisSectionReadCount >= 2 * params.readBytesPerTimeDiff)
                thisSectionReadCount = 0;
            else thisSectionReadCount = params.readBytesPerTimeDiff - thisSectionReadCount;
            if (now >= thisReadTimeSectionEndTime + ServerParams::timeDiff)
                thisReadTimeSectionEndTime = now + ServerParams::timeDiff;
            else thisReadTimeSectionEndTime += ServerParams::timeDiff;
        }
        return n;
    } else return 0;
}

inline void writeBase(int descriptor, const char *buffer, const uint N) {
    if (N == 0) return;
    auto result = 0;
    uint totalBytesRead = 0;
    while (totalBytesRead < N) {
        result = ::send(descriptor, buffer + result, N - result, MSG_NOSIGNAL);
        if (result < 0)
            throw std::runtime_error("ERROR writing to socket");
        totalBytesRead += result;
    }
}

void StreamDescriptor::write(const char *buffer, const uint N) const {
    auto bytesWritten = 0U;
    while (bytesWritten < N) {
        auto bytesToWrite = min(max(1, params.writeBytesPerTimeDiff - thisSectionWriteCount), N);
        writeBase(descriptor, buffer, bytesToWrite);
        bytesWritten += bytesToWrite;
        thisSectionWriteCount += bytesToWrite;
        auto now = preciseNow();
        if (thisSectionWriteCount >= params.writeBytesPerTimeDiff || now >= thisWriteTimeSectionEndTime) {
            std::this_thread::sleep_for(thisWriteTimeSectionEndTime - now);
            if (thisSectionWriteCount >= 2 * params.writeBytesPerTimeDiff)
                thisSectionWriteCount = 0;
            else thisSectionWriteCount = params.writeBytesPerTimeDiff - thisSectionWriteCount;
            if (now >= thisWriteTimeSectionEndTime + ServerParams::timeDiff)
                thisWriteTimeSectionEndTime = now + ServerParams::timeDiff;
            else thisWriteTimeSectionEndTime += ServerParams::timeDiff;
        }
    }
}

void StreamDescriptor::unread(char *extraReadBytes, uint N) {
    if (unreadBytesCount == 0) {
        unreadBytesCount = min(N, MAX_UNREAD_BYTES_COUNT);
        memcpy(unreadBytes, extraReadBytes, unreadBytesCount);
    } else {
        // the extraBytes were read from unreadBytes buffer, re-insert it to front
        auto len = min(N, MAX_UNREAD_BYTES_COUNT - unreadBytesCount);
        memmove(unreadBytes + len, unreadBytes, unreadBytesCount);
        memcpy(unreadBytes, extraReadBytes, len);
        unreadBytesCount += len;
    }
}