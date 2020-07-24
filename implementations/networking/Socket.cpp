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

ssize_t Socket::read(char *buffer, const uint N) const {
    auto result = ::read(socketFd, buffer, N);
    if (result < 0)
        throw std::runtime_error("ERROR reading from socket");
    return result;
}

void Socket::write(const char *buffer, const uint N) const {
    auto result = ::write(socketFd, buffer, N);
    while (result < N) {
        if (result < 0)
            throw std::runtime_error("ERROR writing to socket");
        result += ::write(socketFd, buffer + result, N - result);
    }
}
