#include <thread>
#include "StreamDescriptor.h"

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
        delayRead(n);
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
    auto totalBytesWritten = 0U;
    while (totalBytesWritten < N) {
        auto bytesToWrite = min(max(1, params.writeBytesPerTimeDiff - thisSectionWriteCount), N - totalBytesWritten);
        writeBase(descriptor, buffer + totalBytesWritten, bytesToWrite);
        totalBytesWritten += bytesToWrite;
        delayWrite(bytesToWrite);
    }
}

void StreamDescriptor::unread(const char *extraReadBytes, uint N) {
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

void StreamDescriptor::reset(
        const decltype(preciseNow()) &now,
        decltype(thisSectionReadCount) &thisSectionCount,
        const uint32_t bytesPerTimeDiff,
        decltype(thisReadTimeSectionEndTime) &thisTimeSectionEndTime
)  {
    if (thisSectionCount >= 2 * bytesPerTimeDiff)
        thisSectionCount = 0;
    else thisSectionCount = thisSectionCount - bytesPerTimeDiff;
    if (now >= thisTimeSectionEndTime + ServerParams::timeDiff)
        thisTimeSectionEndTime = now + ServerParams::timeDiff;
    else thisTimeSectionEndTime += ServerParams::timeDiff;
}

void StreamDescriptor::delay(
        ssize_t bytesWritten,
        decltype(thisSectionReadCount) &thisSectionCount,
        const uint32_t bytesPerTimeDiff,
        decltype(thisReadTimeSectionEndTime) &thisTimeSectionEndTime
)  {
    thisSectionCount += bytesWritten;
    auto now = preciseNow();
    if (thisSectionCount >= bytesPerTimeDiff) {
        if (now < thisTimeSectionEndTime)
            std::this_thread::sleep_for(thisTimeSectionEndTime - now);
        reset(now, thisSectionCount, bytesPerTimeDiff, thisTimeSectionEndTime);
    }
    if (now >= thisTimeSectionEndTime)
        reset(now, thisSectionCount, bytesPerTimeDiff, thisTimeSectionEndTime);
}

void StreamDescriptor::delayRead(ssize_t bytesRead) const {
    delay(bytesRead, thisSectionReadCount, params.readBytesPerTimeDiff, thisReadTimeSectionEndTime);
}

void StreamDescriptor::delayWrite(ssize_t bytesWritten) const {
    delay(bytesWritten, thisSectionWriteCount, params.writeBytesPerTimeDiff, thisWriteTimeSectionEndTime);
}
