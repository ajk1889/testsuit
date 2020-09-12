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
    map<string, vector<string>> HEADERS;
    string httpVersion = "1.1";
    uint responseCode = 200;

    explicit HttpResponse(uint code) : responseCode(code), HEADERS() {}

    explicit HttpResponse(uint code, map<string, vector<string>> headers)
            : responseCode(code), HEADERS(std::move(headers)) {}

    virtual Socket &writeTo(Socket &socket) = 0;
};

#endif //TESTSUIT_HTTPRESPONSE_H
