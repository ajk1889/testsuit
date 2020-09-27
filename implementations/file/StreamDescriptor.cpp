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
//        delayRead(n);
        return n;
    } else return 0;
}

inline void writeBase(int descriptor, const char *buffer, const uint N) {
    if (N == 0) return;
    ssize_t result;
    uint totalBytesRead = 0;
    while (totalBytesRead < N) {
        result = ::send(descriptor, buffer + totalBytesRead, N - totalBytesRead, MSG_NOSIGNAL);
        if (result < 0)
            throw std::runtime_error("ERROR writing to socket");
        totalBytesRead += result;
    }
}

void StreamDescriptor::write(const char *buffer, const uint N) const {
    auto bytesWritten = 0U;
    while (bytesWritten < N) {
        auto bytesToWrite = min(max(1, params.writeBytesPerTimeDiff - thisSectionWriteCount), N - bytesWritten);
        writeBase(descriptor, buffer + bytesWritten, bytesToWrite);
        bytesWritten += bytesToWrite;
//        delayWrite(bytesToWrite);
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


void StreamDescriptor::delayRead(ssize_t bytesRead) const {
    thisSectionReadCount += bytesRead;
    auto now = preciseNow();
    if (thisSectionReadCount >= params.readBytesPerTimeDiff) {
        if (now < thisReadTimeSectionEndTime)
            std::this_thread::sleep_for(thisReadTimeSectionEndTime - now);
        if (thisSectionReadCount >= 2 * params.readBytesPerTimeDiff)
            thisSectionReadCount = 0;
        else thisSectionReadCount = params.readBytesPerTimeDiff - thisSectionReadCount;
        if (now >= thisReadTimeSectionEndTime + ServerParams::timeDiff)
            thisReadTimeSectionEndTime = now + ServerParams::timeDiff;
        else thisReadTimeSectionEndTime += ServerParams::timeDiff;
    }
    if (now >= thisReadTimeSectionEndTime) {
        if (thisSectionReadCount >= 2 * params.readBytesPerTimeDiff)
            thisSectionReadCount = 0;
        else thisSectionReadCount = params.readBytesPerTimeDiff - thisSectionReadCount;
        if (now >= thisReadTimeSectionEndTime + ServerParams::timeDiff)
            thisReadTimeSectionEndTime = now + ServerParams::timeDiff;
        else thisReadTimeSectionEndTime += ServerParams::timeDiff;
    }
}

void StreamDescriptor::delayWrite(ssize_t bytesWritten) const {
    thisSectionWriteCount += bytesWritten;
    auto now = preciseNow();
    if (thisSectionWriteCount >= params.writeBytesPerTimeDiff) {
        if (now < thisWriteTimeSectionEndTime)
            std::this_thread::sleep_for(thisWriteTimeSectionEndTime - now);
        if (thisSectionWriteCount >= 2 * params.writeBytesPerTimeDiff)
            thisSectionWriteCount = 0;
        else thisSectionWriteCount = params.writeBytesPerTimeDiff - thisSectionWriteCount;
        if (now >= thisWriteTimeSectionEndTime + ServerParams::timeDiff)
            thisWriteTimeSectionEndTime = now + ServerParams::timeDiff;
        else thisWriteTimeSectionEndTime += ServerParams::timeDiff;
    }
    if (now >= thisWriteTimeSectionEndTime) {
        if (thisSectionWriteCount >= 2 * params.writeBytesPerTimeDiff)
            thisSectionWriteCount = 0;
        else thisSectionWriteCount = params.writeBytesPerTimeDiff - thisSectionWriteCount;
        if (now >= thisWriteTimeSectionEndTime + ServerParams::timeDiff)
            thisWriteTimeSectionEndTime = now + ServerParams::timeDiff;
        else thisWriteTimeSectionEndTime += ServerParams::timeDiff;
    }
}