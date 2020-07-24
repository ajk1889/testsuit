#ifndef TESTSUIT_SERVERSOCKET_H
#define TESTSUIT_SERVERSOCKET_H

#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>

#include "Socket.h"

class ServerSocket {
    int serverSocketFd;
    uint maxParallelConns = 1;
    short port;
public:
    explicit ServerSocket(short portNo = 1234, uint maxParallelConnections = 10);

    Socket accept() const;

    void close() const { ::close(serverSocketFd); }

    ~ServerSocket() { close(); }
};


#endif //TESTSUIT_SERVERSOCKET_H
