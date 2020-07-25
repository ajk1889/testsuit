#include "Request.h"

ssize_t Request::read(char *buffer, uint N) const {
    auto copyLen = std::min(N, extraReadBytesLen - extraReadBytesOffset);
    if (copyLen > 0) {
        memcpy(buffer, extraReadBytes + extraReadBytesOffset, copyLen);
        extraReadBytesOffset += copyLen;
        buffer += copyLen;
        N -= copyLen;
    }
    return copyLen + socket->read(buffer, N);
}

void Request::setHeaders(const Socket &client) {
    char buffer[BUFSIZ];
    auto n = client.read(buffer, BUFSIZ);
    while (n) {
        // todo
        n = client.read(buffer, BUFSIZ);
    }
}
