//
// Created by ajk on 11/09/20.
//

#include "FileResponse.h"

Socket &FileResponse::writeTo(Socket &socket) {
    HttpResponse::writeTo(socket);
    char buffer[BUFFER_SIZE + 1];
    uint_least64_t bytesRead = 0;
    auto length = end - begin + 1;
    while (bytesRead < length && stream.read(buffer, min(BUFFER_SIZE, length - bytesRead))) {
        bytesRead += stream.gcount();
        socket.write(buffer, bytesRead);
    }
    return socket;
}
