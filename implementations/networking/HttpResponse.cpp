#include "HttpResponse.h"
#include "Socket.h"

HttpResponse::HttpResponse(const string &data) : HttpResponse(map<string, vector<string>>(), data) {
    std::ostringstream defaultHeaders("HTTP/1.1 200 OK\r\n");
    defaultHeaders << "Date: " << currentDateTime() << "\r\n";
    defaultHeaders << "Server: " SERVER_NAME "\r\n";
    static std::hash<string> hasher;
    defaultHeaders << "ETag: " << hasher(data) << "\r\n";
    defaultHeaders << "Content-Type: text/html; charset=UTF-8\r\n\r\n";

    parseHttpHeader(defaultHeaders.str(), HEADERS);
}

Socket &operator<<(Socket &socket, const HttpResponse &response) {
    socket.write("HTTP/1.1 200 OK\r\n");
    for (const auto &pair : response.HEADERS)
        for (const auto &value: pair.second)
            socket.write(pair.first + ": " + value + "\r\n");
    socket.write("\r\n");
    istream &is = *response.dataStream;
    char buffer[BUFFER_SIZE];
    auto len = response.streamLength;
    while (is.read(buffer, BUFFER_SIZE) && len > BUFFER_SIZE) {
        len -= BUFFER_SIZE;
        socket.write(buffer, BUFFER_SIZE);
    }
    socket.write(buffer, len);
    return socket;
}
