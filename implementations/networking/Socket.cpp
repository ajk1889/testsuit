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

ssize_t Socket::read(char *buffer, uint N) {
    decltype(min(N, unreadBytesCount)) availableBytesLen = 0;
    if (N && unreadBytesCount) {
        availableBytesLen = min(N, unreadBytesCount);
        unreadBytesCount -= availableBytesLen;
        N -= availableBytesLen;
        memcpy(buffer, unreadBytes, availableBytesLen);
        memmove(const_cast<char *>(unreadBytes),
                unreadBytes + availableBytesLen, unreadBytesCount);
    }
    if (N) return ::read(socketFd, buffer, N) + availableBytesLen;
    else return availableBytesLen;
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
