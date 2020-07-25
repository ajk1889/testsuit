#include "ServerSocket.h"

ServerSocket::ServerSocket(short portNo, uint maxParallelConnections)
        : port(portNo), maxParallelConns(maxParallelConnections) {
    serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketFd < 0)
        throw std::runtime_error("ERROR opening socket");
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(serverSocketFd, (sockaddr *) &address, sizeof(address)) < 0)
        throw std::runtime_error("ERROR on binding");
    listen(serverSocketFd, maxParallelConns);
}

std::shared_ptr<Socket> ServerSocket::accept() const {
    sockaddr_in clientAddress{};
    socklen_t size = sizeof(clientAddress);
    auto socketFd = ::accept(serverSocketFd, (sockaddr *) &clientAddress, &size);
    if (socketFd < 0)
        throw std::runtime_error("ERROR while accepting");
    return std::make_shared<Socket>(socketFd);
}
