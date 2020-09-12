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
#include "../file/FileOrString.h"
#include "json/json.hpp"
#include <curl/curl.h>
#include <curl/easy.h>

using json = nlohmann::json;
using std::string;
using std::map;
using std::vector;
using std::shared_ptr;

class HttpRequest {
    friend class Socket;

    explicit HttpRequest(shared_ptr<Socket> client) : socket(std::move(client)) {}

public:
    map<string, vector<string>> HEADERS;
    map<string, string> GET;
    map<string, FileOrString> POST;
    string httpVersion;
    string path;
    string requestType;
    shared_ptr<Socket> socket;

    ssize_t read(char *buffer, uint N) const {
        return socket->read(buffer, N);
    }

    static HttpRequest from(const shared_ptr<Socket> &client);

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(HttpRequest, requestType, path, GET, POST, HEADERS, httpVersion);
};


#endif //TESTSUIT_HTTPREQUEST_H
