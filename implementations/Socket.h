//
// Created by ajk on 20/07/20.
//

#ifndef TESTSUIT_SOCKET_H
#define TESTSUIT_SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <unistd.h>
#include <stdexcept>
#include <sys/types.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netdb.h>

using std::string;

class Socket {
    friend class ServerSocket;

    int socketFd;

    explicit Socket(int fd) : socketFd(fd) {}

public:
    Socket(const string &ip, short port);

    template<unsigned int N>
    ssize_t write(const char (&buffer)[N]) const {
        auto result = ::write(socketFd, buffer, N);
        if (result < 0)
            throw std::runtime_error("ERROR writing to socket");
        return result;
    }

    ssize_t write(const char *buffer, uint N) const;

    template<unsigned int N>
    ssize_t read(char (&buffer)[N]) const {
        auto result = ::read(socketFd, buffer, N);
        if (result < 0)
            throw std::runtime_error("ERROR reading from socket");
        return result;
    }

    ssize_t read(char *buffer, uint N) const;

    void close() const {
        ::close(socketFd);
    }
};


#endif //TESTSUIT_SOCKET_H
