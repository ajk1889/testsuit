#ifndef TESTSUIT_STREAMDESCRIPTOR_H
#define TESTSUIT_STREAMDESCRIPTOR_H

#include <unistd.h>
#include <string>
#include "../constants.h"
#include "../helpers.h"
#include "../../server/ServerParams.h"
#include <cstring>

using std::string;

class StreamDescriptor {
    friend class Socket;

    int descriptor;
    constexpr static auto MAX_UNREAD_BYTES_COUNT = 8 * KB;
    char unreadBytes[MAX_UNREAD_BYTES_COUNT]{};
    uint unreadBytesCount = 0;

    mutable decltype(preciseNow()) thisWriteTimeSectionEndTime;
    mutable decltype(preciseNow()) thisReadTimeSectionEndTime;
    mutable int32_t thisSectionWriteCount = 0;
    mutable int32_t thisSectionReadCount = 0;
public:
    StreamDescriptor(int fd) : descriptor(fd) {}

    template<unsigned int N>
    void write(const char (&buffer)[N]) const {
        write(buffer, N - 1);
    }

    template<typename T>
    void write(const T &object) const {
        write(reinterpret_cast<const char *>(&object), sizeof(T));
    }

    void write(const string &str) const {
        write(str.c_str(), str.length());
    }

    void write(const char *buffer, uint N) const;

    template<unsigned int N>
    ssize_t read(char (&buffer)[N]) const {
        return read(buffer, N - 1);
    }

    template<typename T>
    T &read(T &to) {
        auto toPtr = reinterpret_cast<char *>(&to);
        read(toPtr, sizeof(T));
        return to;
    }

    void unread(const char *extraReadBytes, uint N);

    ssize_t read(char *buffer, uint N);

    void close() const { ::close(descriptor); }

    void delayRead(ssize_t bytesRead) const;

    static void reset(
            const decltype(preciseNow()) &now,
            decltype(thisSectionReadCount) &thisSectionCount,
            const uint32_t bytesPerTimeDiff,
            decltype(thisReadTimeSectionEndTime) &thisTimeSectionEndTime
    );

    static void delay(
            ssize_t bytes,
            decltype(thisSectionReadCount) &thisSectionCount,
            const uint32_t bytesPerTimeDiff,
            decltype(thisReadTimeSectionEndTime) &thisTimeSectionEndTime
    );

    void delayWrite(ssize_t bytesWritten) const;

    ~StreamDescriptor() {
        close();
    }
};


#endif //TESTSUIT_STREAMDESCRIPTOR_H
