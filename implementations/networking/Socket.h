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
#include <memory>
#include <cstring>
#include "../helpers.h"
#include "../constants.h"

using std::string;

class Server;

class Socket {
    friend class ServerSocket;

    friend class Server;

    int socketFd;
    constexpr static auto MAX_UNREAD_BYTES_COUNT = 8 * KB;
    char unreadBytes[MAX_UNREAD_BYTES_COUNT]{};
    uint unreadBytesCount = 0;
public:
    const Server *server = nullptr;
    Socket(const Socket &) = delete;

    Socket(int fd, const Server *server) : socketFd(fd), server(server) {}

    Socket(const string &ip, short port);

    template<unsigned int N>
    void write(const char (&buffer)[N]) const {
        auto result = ::write(socketFd, buffer, N - 1);
        while (result < N - 1) {
            if (result < 0)
                throw std::runtime_error("ERROR writing to socket");
            result += ::write(socketFd, buffer + result, N - result - 1);
        }
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
        return read(buffer, N);
    }

    template<typename T>
    T &read(T &to) {
        auto toPtr = reinterpret_cast<char *>(&to);
        constexpr auto N = sizeof(T);
        auto result = read(toPtr, N);
        while (result < N) {
            if (result < 0)
                throw std::runtime_error("ERROR reading from socket");
            result += read(toPtr + result, N - result);
        }
        return to;
    }

    void unread(char *extraReadBytes, uint N);

    ssize_t read(char *buffer, uint N);

    void close() const { ::close(socketFd); }

    ~Socket() { close(); }
};


#endif //TESTSUIT_SOCKET_H
