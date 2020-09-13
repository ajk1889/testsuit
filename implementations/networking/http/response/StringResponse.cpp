#include "StringResponse.h"

Socket &StringResponse::writeTo(Socket &socket) {
    HttpResponse::writeTo(socket);
    socket.write(responseString);
    return socket;
}

StringResponse::StringResponse(uint responseCode, const string &data)
        : HttpResponse(responseCode), responseString(data) {
    HEADERS["Content-Length"].push_back(std::to_string(data.length()));
    HEADERS["Content-Type"].push_back("text/html; charset=UTF-8");
}

StringResponse::StringResponse(uint responseCode, const string &data, const decltype(HEADERS) &additionalHeaders)
        : HttpResponse(responseCode), responseString(data) {
    HEADERS["Content-Length"].push_back(std::to_string(data.length()));
    HEADERS["Content-Type"].push_back("text/html; charset=UTF-8");
    for (auto &pair: additionalHeaders) HEADERS[pair.first] = pair.second;
}
