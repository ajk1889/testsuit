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
            const decltype(HEADERS) additionalHeaders,
            const uint_least64_t length
    ) : HttpResponse(responseCode), path(std::move(filePath)), size(length) {}

    Socket &writeTo(Socket &socket) override;
};


#endif //TESTSUIT_FILERESPONSE_H
