#ifndef TESTSUIT_SERVERSOCKET_H
#define TESTSUIT_SERVERSOCKET_H

#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <memory>
#include <poll.h>
#include <thread>
#include "Socket.h"

class Server;

class ServerSocket {
    int serverSocketFd;
    uint maxParallelConns = 1;
    short port;
public:
    const Server *server = nullptr;

    ServerSocket(const ServerSocket &) = delete;

    explicit ServerSocket(const Server *server = nullptr, short portNo = 1234, uint maxParallelConnections = 10);

    [[nodiscard]] std::shared_ptr<Socket> accept(timeval timeOut) const;

    void close() const {
        using std::chrono_literals::operator ""ms;
        std::this_thread::sleep_for(2ms);
        ::close(serverSocketFd);
    }

    ~ServerSocket() {
        close();
    }
};


#endif //TESTSUIT_SERVERSOCKET_H
