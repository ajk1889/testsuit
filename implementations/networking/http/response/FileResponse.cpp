#include "../../../../server/Server.h"
#include "FileResponse.h"

Socket &FileResponse::writeTo(Socket &socket) {
    HttpResponse::writeTo(socket);
    char buffer[BUFFER_SIZE + 1]{};
    uint_least64_t totalBytesRead = 0;
    auto length = end - begin + 1;
    while (totalBytesRead < length && stream.read(buffer, min(BUFFER_SIZE, length - totalBytesRead))) {
        auto bytesRead = stream.gcount();
        totalBytesRead += bytesRead;
        socket.write(buffer, bytesRead);
    }
    return socket;
}
