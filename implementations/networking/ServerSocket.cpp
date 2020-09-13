#include "ServerSocket.h"

ServerSocket::ServerSocket(const Server *server, short portNo, uint maxParallelConnections)
        : port(portNo), maxParallelConns(maxParallelConnections), server(server) {
    serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketFd < 0)
        throw std::runtime_error("ERROR opening socket");
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    int reusePort = 1;
    setsockopt(serverSocketFd, SOL_SOCKET, SO_REUSEPORT, &reusePort, sizeof(reusePort));
    if (bind(serverSocketFd, (sockaddr *) &address, sizeof(address)) < 0)
        throw std::runtime_error("ERROR on binding");
    listen(serverSocketFd, maxParallelConns);
}

std::shared_ptr<Socket> ServerSocket::accept(timeval timeOut) const {
    fd_set readFds;
    FD_SET(serverSocketFd, &readFds);
    auto result = select(serverSocketFd + 1, &readFds, nullptr, nullptr, &timeOut);
    if (result == 1) {
        sockaddr_in clientAddress{};
        socklen_t size = sizeof(clientAddress);
        auto socketFd = ::accept(serverSocketFd, (sockaddr *) &clientAddress, &size);
        if (socketFd < 0) return std::shared_ptr<Socket>();
        return std::make_shared<Socket>(socketFd, this->server);
    } else return std::shared_ptr<Socket>();
}
