//
// Created by ajk on 11/09/20.
//

#ifndef TESTSUIT_FILERESPONSE_H
#define TESTSUIT_FILERESPONSE_H


#include "HttpResponse.h"

class FileResponse : public HttpResponse {
    const string path;
    const uint_least64_t size;
public:
    FileResponse(
            uint responseCode,
            string filePath,
            const decltype(HEADERS) &additionalHeaders,
            const uint_least64_t length
    ) : HttpResponse(responseCode, additionalHeaders), path(std::move(filePath)), size(length) {
        if (size != 0) HEADERS["Content-Length"].push_back(std::to_string(size));
    }

    Socket &writeTo(Socket &socket) override;
};


#endif //TESTSUIT_FILERESPONSE_H
