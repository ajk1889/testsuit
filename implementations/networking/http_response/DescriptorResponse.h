//
// Created by ajk on 11/09/20.
//

#ifndef TESTSUIT_DESCRIPTORRESPONSE_H
#define TESTSUIT_DESCRIPTORRESPONSE_H


#include "HttpResponse.h"

class DescriptorResponse : public HttpResponse {
    StreamDescriptor descriptor;
public:
    DescriptorResponse(uint responseCode, const StreamDescriptor descriptor)
            : HttpResponse(responseCode), descriptor(descriptor) {}
    Socket &writeTo(Socket &socket) override;
};


#endif //TESTSUIT_DESCRIPTORRESPONSE_H
