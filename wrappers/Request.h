#ifndef TESTSUIT_REQUEST_H
#define TESTSUIT_REQUEST_H

#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <memory>
#include "../implementations/networking/Socket.h"

using std::string;
using std::map;
using std::vector;
using std::shared_ptr;

enum RequestType {
    GET, POST, PUT, DELETE, HEAD
};

class Request {
    char extraReadBytes[BUFSIZ];
    uint extraReadBytesLen;
    mutable uint extraReadBytesOffset = 0;

    void setHeaders(const Socket &client);

    static int headerTerminationPoint(const char *buffer, uint len, const char (&last3)[3]) {
        if (len == 0) return -1;
        static char requiredLast3[3] = {'\r', '\n', '\r'};
        if (buffer[0] == '\n' && last3 == requiredLast3)
            return 1;
        len -= 4;
        for (int i = 0; i < len; ++i, ++buffer) {
            if (buffer[0] == '\r' && buffer[1] == '\n' && buffer[2] == '\r' && buffer[3] == '\n')
                return i + 4;
        }
        return -1;
    }

public:
    map<string, vector<string>> HEADERS;
    map<string, string> GET;
    map<string, string> POST;
    RequestType requestType;

    ssize_t read(char *buffer, uint N) const;

    shared_ptr<Socket> socket;

    Request(shared_ptr<Socket> &client) : socket(client) {

    }
};


#endif //TESTSUIT_REQUEST_H
