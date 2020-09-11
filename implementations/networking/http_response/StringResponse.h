//
// Created by ajk on 11/09/20.
//

#ifndef TESTSUIT_STRINGRESPONSE_H
#define TESTSUIT_STRINGRESPONSE_H


#include "HttpResponse.h"

class StringResponse : public HttpResponse {
public:
    string responseString;

    StringResponse(uint responseCode, const string &data);
};

Socket &operator<<(Socket &socket, const StringResponse &response);

#endif //TESTSUIT_STRINGRESPONSE_H
