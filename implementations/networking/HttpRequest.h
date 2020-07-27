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

    static int headerTerminationPoint(const char *buffer, uint len, const char (&last3)[3]);

public:
    map<string, vector<string>> HEADERS;
    map<string, string> GET;
    map<string, string> POST;
    string httpVersion;
    string path;
    string requestType;
    CURL *curl = nullptr;

    ssize_t read(char *buffer, uint N) const;

    string extractRawHeaders();

    void extractGetParams(const string &basicString);

    void setRequestParams();

    shared_ptr<Socket> socket;

    explicit HttpRequest(shared_ptr<Socket> client) : socket(std::move(client)), curl(curl_easy_init()) {}

    void extractHeaderKeyValues(const string &headerKeyValues);
};


#endif //TESTSUIT_HTTPREQUEST_H
