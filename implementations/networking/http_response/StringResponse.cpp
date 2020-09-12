//
// Created by ajk on 11/09/20.
//

#include "StringResponse.h"

Socket &StringResponse::writeTo(Socket &socket) {
    socket.write("HTTP/" + httpVersion + " ");
    socket.write(std::to_string(responseCode));
    socket.write(" " + ResponseCode::text.find(responseCode)->second + "\r\n");
    for (const auto &pair : HEADERS)
        for (const auto &value: pair.second)
            socket.write(pair.first + ": " + value + "\r\n");
    socket.write("\r\n");
    socket.write(responseString);
    return socket;
}

StringResponse::StringResponse(uint responseCode, const string &data)
        : HttpResponse(responseCode, {
        {"Date",           {currentDateTime()}},
        {"Server",         {SERVER_NAME}},
        {"Content-Length", {std::to_string(data.length())}},
        {"Content-Type",   {"text/html; charset=UTF-8"}}
}), responseString(data) {}
