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
#include "../constants.h"
#include <curl/curl.h>
#include <curl/easy.h>

using std::string;
using std::map;
using std::vector;
using std::shared_ptr;

class HttpRequest {
    friend class Socket;

    static int headerTerminationPoint(const char *buffer, int len, const char (&last3)[3]);

    string extractRawHeaders();

    void extractHeaderKeyValues(const string &headerKeyValues);

    explicit HttpRequest(shared_ptr<Socket> client) : socket(std::move(client)) {}

public:
    map<string, vector<string>> HEADERS;
    map<string, string> GET;
    map<string, string> POST;
    string httpVersion;
    string path;
    string requestType;
    shared_ptr<Socket> socket;

    ssize_t read(char *buffer, uint N) const {
        return socket->read(buffer, N);
    }


    static HttpRequest from(const shared_ptr<Socket> &client);
};


#endif //TESTSUIT_HTTPREQUEST_H
