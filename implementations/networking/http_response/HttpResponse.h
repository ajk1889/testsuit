#ifndef TESTSUIT_HTTPRESPONSE_H
#define TESTSUIT_HTTPRESPONSE_H

#include <string>
#include <utility>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include "../../helpers.h"
#include "../Socket.h"

using std::string;
using std::vector;
using std::map;
using std::istream;
using std::ostream;
using std::istringstream;

struct HttpResponse {
    map<string, vector<string>> HEADERS = {
            {"Date",   {currentDateTime()}},
            {"Server", {SERVER_NAME}}
    };
    string httpVersion = "1.1";
    uint responseCode = 200;

    explicit HttpResponse(uint code) : responseCode(code), HEADERS() {}

    explicit HttpResponse(uint code, const map<string, vector<string>> &additionalHeaders) : responseCode(code) {
        for (auto &pair: additionalHeaders) HEADERS[pair.first] = pair.second;
    }

    virtual Socket &writeTo(Socket &socket) {
        socket.write("HTTP/" + httpVersion + " ");
        socket.write(std::to_string(responseCode));
        socket.write(" " + ResponseCode::text.find(responseCode)->second + "\r\n");
        for (const auto &pair : HEADERS)
            for (const auto &value: pair.second)
                socket.write(pair.first + ": " + value + "\r\n");
        socket.write("\r\n");
        return socket;
    }
};

#endif //TESTSUIT_HTTPRESPONSE_H
