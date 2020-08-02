#include <iostream>
#include "Socket.h"

Socket::Socket(const string &ip, short port) : socketFd(socket(AF_INET, SOCK_STREAM, 0)) {
    if (socketFd < 0)
        throw std::runtime_error("ERROR opening socket");
    sockaddr_in address{};
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &(address.sin_addr));
    address.sin_port = htons(port);
    if (connect(socketFd, reinterpret_cast<sockaddr *>(&address), sizeof(address)) < 0)
        throw std::runtime_error("ERROR connecting");
}

ssize_t Socket::read(char *buffer, const uint N) {
    decltype(min(N, unreadBytesCount)) availableBytesLen = 0;
    if (N && unreadBytesCount) {
        availableBytesLen = min(N, unreadBytesCount);
        unreadBytesCount -= availableBytesLen;
        memcpy(buffer, unreadBytes, availableBytesLen);
        memmove(const_cast<char *>(unreadBytes),
                unreadBytes + availableBytesLen, unreadBytesCount);
        return availableBytesLen;
    } else if (N) {
        return ::read(socketFd, buffer, N);
    } else return 0;
}

void Socket::write(const char *buffer, const uint N) const {
    if (N == 0) return;
    auto result = ::write(socketFd, buffer, N);
    while (result < N) {
        if (result < 0)
            throw std::runtime_error("ERROR writing to socket");
        result += ::write(socketFd, buffer + result, N - result);
    }
}

void Socket::unread(char *extraReadBytes, uint N) {
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
