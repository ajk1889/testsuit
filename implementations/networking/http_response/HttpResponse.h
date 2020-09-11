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

    explicit HttpResponse(uint code) : responseCode(code) {}
};

Socket &operator<<(Socket &socket, const HttpResponse &response);

#endif //TESTSUIT_HTTPRESPONSE_H
