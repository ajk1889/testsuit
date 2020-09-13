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
#include "../file/StreamDescriptor.h"

using std::string;

class Server;

class Socket : public StreamDescriptor {
    friend class ServerSocket;

    friend class Server;

public:
    const Server *server = nullptr;

    Socket(const Socket &) = delete;

    Socket(int fd, const Server *server) : StreamDescriptor(fd), server(server) {}

    Socket(const string &ip, short port);

    ~Socket() { close(); }
};


#endif //TESTSUIT_SOCKET_H
