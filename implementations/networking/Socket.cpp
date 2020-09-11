#include <iostream>
#include "Socket.h"

Socket::Socket(const string &ip, short port) : StreamDescriptor(socket(AF_INET, SOCK_STREAM, 0)) {
    if (descriptor < 0)
        throw std::runtime_error("ERROR opening socket");
    sockaddr_in address{};
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &(address.sin_addr));
    address.sin_port = htons(port);
    if (connect(descriptor, reinterpret_cast<sockaddr *>(&address), sizeof(address)) < 0)
        throw std::runtime_error("ERROR connecting");
}
