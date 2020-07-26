#ifndef TESTSUIT_HTTPREQUEST_H
#define TESTSUIT_HTTPREQUEST_H

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <cstring>
#include <cmath>
#include <memory>
#include "../ArrayJoiner.h"
#include "Socket.h"

using std::string;
using std::map;
using std::vector;
using std::shared_ptr;

enum RequestType {
    GET, POST, PUT, DELETE, HEAD
};

class HttpRequest {
    friend class Socket;

    char extraReadBytes[BUFSIZ]{};
    uint extraReadBytesLen = 0;
    mutable uint extraReadBytesOffset = 0;

    static int headerTerminationPoint(const char *buffer, uint len, const char (&last3)[3]);

public:
    map<string, vector<string>> HEADERS;
    map<string, string> GET;
    map<string, string> POST;
    RequestType requestType = RequestType::GET;

    ssize_t read(char *buffer, uint N) const;

    string extractRawHeaders();

    shared_ptr<Socket> socket;

    explicit HttpRequest(shared_ptr<Socket> client) : socket(std::move(client)) {}
};


#endif //TESTSUIT_HTTPREQUEST_H
