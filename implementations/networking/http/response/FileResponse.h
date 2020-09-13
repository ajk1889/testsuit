//
// Created by ajk on 11/09/20.
//

#ifndef TESTSUIT_FILERESPONSE_H
#define TESTSUIT_FILERESPONSE_H


#include "HttpResponse.h"
#include <fstream>

using std::ifstream;
using std::to_string;

class FileResponse : public HttpResponse {
    ifstream stream;
    uint_least64_t begin, end;
public:
    FileResponse(
            uint responseCode,
            const string &filePath,
            const decltype(HEADERS) &additionalHeaders,
            const uint_least64_t offset,
            const uint_least64_t limit) :
            HttpResponse(responseCode, additionalHeaders),
            stream(filePath, std::ifstream::ate | std::ifstream::binary),
            begin(offset), end(limit) {
        auto fileSize = stream.tellg();
        if (end == 0)
            end = static_cast<uint_least64_t>(fileSize) - 1ULL;
        HEADERS["Content-Length"].push_back(to_string(end - begin + 1));
        HEADERS["Content-Range"].push_back(
                "bytes " + to_string(begin) + "-" + to_string(end) + "/" + to_string(fileSize));
        stream.seekg(begin);
    }

    Socket &writeTo(Socket &socket) override;
};


#endif //TESTSUIT_FILERESPONSE_H
