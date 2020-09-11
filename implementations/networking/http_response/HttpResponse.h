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

class HttpResponse {
    friend Socket &operator<<(Socket &socket, const HttpResponse &response);

public:
    map<string, vector<string>> HEADERS;
    string httpVersion = "1.1";
    uint responseCode = 200;
    bool destroyDataStream = false;
    string data;
    ulong streamLength;

    explicit HttpResponse(uint responseCode, const string &data);

    HttpResponse(decltype(HEADERS) headers, string response) :
            HEADERS(std::move(headers)), data(std::move(response)), destroyDataStream(true) {
        streamLength = this->data.length();
        HEADERS["Content-Length"].push_back(std::to_string(streamLength));
    }
};

Socket &operator<<(Socket &socket, const HttpResponse &response);

ostream &operator<<(ostream &os, const HttpResponse &response);

#endif //TESTSUIT_HTTPRESPONSE_H
