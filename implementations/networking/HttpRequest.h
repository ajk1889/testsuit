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
#include <curl/curl.h>
#include <curl/easy.h>

using std::string;
using std::map;
using std::vector;
using std::shared_ptr;

class HttpRequest {
    friend class Socket;

    char extraReadBytes[BUFSIZ]{};
    uint extraReadBytesLen = 0;
    mutable uint extraReadBytesOffset = 0;
    CURL *curl = nullptr;

    static int headerTerminationPoint(const char *buffer, int len, const char (&last3)[3]);

    string extractRawHeaders();

    void extractGetParams(const string &basicString);

    void extractHeaderKeyValues(const string &headerKeyValues);

public:
    map<string, vector<string>> HEADERS;
    map<string, string> GET;
    map<string, string> POST;
    string httpVersion;
    string path;
    string requestType;
    shared_ptr<Socket> socket;

    ssize_t read(char *buffer, uint N) const;

    explicit HttpRequest(shared_ptr<Socket> client);
};


#endif //TESTSUIT_HTTPREQUEST_H
