#ifndef TESTSUIT_HTTPRESPONSE_H
#define TESTSUIT_HTTPRESPONSE_H

#include <string>
#include <utility>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include "../helpers.h"

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
    istream *dataStream;
    ulong streamLength;

    explicit HttpResponse(const string &data);

    HttpResponse(decltype(HEADERS) headers, const string &data) :
            HEADERS(std::move(headers)), dataStream(new istringstream(data)), destroyDataStream(true) {
        dataStream->seekg(0, istream::end);
        streamLength = dataStream->tellg();
        dataStream->seekg(0, istream::beg);
        HEADERS["Content-Length"].push_back(std::to_string(streamLength));
    }

    ~HttpResponse() {
        if (destroyDataStream) delete dataStream;
    }
};

Socket &operator<<(Socket &socket, const HttpResponse &response);

ostream &operator<<(ostream &os, const HttpResponse &response);

#endif //TESTSUIT_HTTPRESPONSE_H
