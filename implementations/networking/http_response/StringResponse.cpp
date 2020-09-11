//
// Created by ajk on 11/09/20.
//

#include "StringResponse.h"

Socket &operator<<(Socket &socket, const StringResponse &response) {
    socket.write("HTTP/" + response.httpVersion + " ");
    socket.write(std::to_string(response.responseCode));
    socket.write(" " + ResponseCode::text.find(response.responseCode)->second + "\r\n");
    for (const auto &pair : response.HEADERS)
        for (const auto &value: pair.second)
            socket.write(pair.first + ": " + value + "\r\n");
    socket.write("\r\n");
    socket.write(response.responseString);
    return socket;
}

StringResponse::StringResponse(uint responseCode, const string &data)
        : HttpResponse(responseCode), responseString(data) {
    std::ostringstream defaultHeaders("HTTP/1.1 200 OK\r\n");
    defaultHeaders << "Date: " << currentDateTime() << "\r\n";
    defaultHeaders << "Server: " SERVER_NAME "\r\n";
    static std::hash<string> hasher;
    defaultHeaders << "ETag: " << hasher(data) << "\r\n";
    defaultHeaders << "Content-Length: " << data.length() << "\r\n";
    defaultHeaders << "Content-Type: text/html; charset=UTF-8\r\n\r\n";
    parseHttpHeader(defaultHeaders.str(), HEADERS);
    this->responseCode = responseCode;
}
