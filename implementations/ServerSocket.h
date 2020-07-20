//
// Created by ajk on 20/07/20.
//

#ifndef TESTSUIT_SERVERSOCKET_H
#define TESTSUIT_SERVERSOCKET_H

#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>

class ServerSocket {
    int socketFd;
    uint maxParallelConns = 1;
    short port;

    explicit ServerSocket(short portNo = 1234, uint maxParallelConnections = 10)
            : port(portNo), maxParallelConns(maxParallelConnections) {
        socketFd = socket(AF_INET, SOCK_STREAM, 0);
        if (socketFd < 0)
            throw std::runtime_error("ERROR opening socket");
        sockaddr_in serv_addr{};
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(port);
        if (bind(socketFd, (sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
            throw std::runtime_error("ERROR on binding");
        listen(socketFd, maxParallelConns);
    }

    void accept() {
    }
};


#endif //TESTSUIT_SERVERSOCKET_H
