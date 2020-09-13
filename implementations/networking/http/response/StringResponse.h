#ifndef TESTSUIT_STRINGRESPONSE_H
#define TESTSUIT_STRINGRESPONSE_H


#include "HttpResponse.h"

class StringResponse : public HttpResponse {
public:
    string responseString;

    StringResponse(uint responseCode, const string &data);

    StringResponse(uint responseCode, const string &data, const decltype(HEADERS) &additionalHeaders);

    Socket &writeTo(Socket &socket) override;
};

Socket &operator<<(Socket &socket, const StringResponse &response);

#endif //TESTSUIT_STRINGRESPONSE_H
