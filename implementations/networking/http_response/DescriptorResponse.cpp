//
// Created by ajk on 11/09/20.
//

#include "DescriptorResponse.h"

Socket &DescriptorResponse::writeTo(Socket &socket) {
    HttpResponse::writeTo(socket);
    char buffer[BUFFER_SIZE + 1];
    ssize_t bytesRead;
    while ((bytesRead = descriptor.read(buffer, BUFFER_SIZE)) > 0)
        socket.write(buffer, bytesRead);
    return socket;
}
