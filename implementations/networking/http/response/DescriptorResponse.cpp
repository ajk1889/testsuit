#include "DescriptorResponse.h"

Socket &DescriptorResponse::writeTo(Socket &socket) {
    HttpResponse::writeTo(socket);
    char buffer[BUFFER_SIZE + 1]{};
    ssize_t n;
    uint_least64_t totalBytesRead = 0;
    decltype(length) localLength = length ? length : UINT64_MAX;
    while (totalBytesRead < localLength &&
           (n = descriptor.read(buffer, min(BUFFER_SIZE, localLength - totalBytesRead))) > 0) {
        totalBytesRead += n;
        socket.write(buffer, n);
    }
    return socket;
}
