#include "HttpResponse.h"
#include "../Socket.h"

HttpResponse::HttpResponse(uint responseCode, const string &data)
        : HttpResponse(map<string, vector<string>>(), data) {
    std::ostringstream defaultHeaders("HTTP/1.1 200 OK\r\n");
    defaultHeaders << "Date: " << currentDateTime() << "\r\n";
    defaultHeaders << "Server: " SERVER_NAME "\r\n";
    static std::hash<string> hasher;
    defaultHeaders << "ETag: " << hasher(data) << "\r\n";
    defaultHeaders << "Content-Type: text/html; charset=UTF-8\r\n\r\n";

    parseHttpHeader(defaultHeaders.str(), HEADERS);
    this->responseCode = responseCode;
}

Socket &operator<<(Socket &socket, const HttpResponse &response) {
    socket.write("HTTP/1.1 ");
    socket.write(std::to_string(response.responseCode));
    socket.write(" " + ResponseCode::text.find(response.responseCode)->second + "\r\n");
    for (const auto &pair : response.HEADERS)
        for (const auto &value: pair.second)
            socket.write(pair.first + ": " + value + "\r\n");
    socket.write("\r\n");
    socket.write(response.data);
    return socket;
}
