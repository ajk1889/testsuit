//
// Created by ajk on 11/09/20.
//

#ifndef TESTSUIT_DESCRIPTORRESPONSE_H
#define TESTSUIT_DESCRIPTORRESPONSE_H


#include "HttpResponse.h"

class DescriptorResponse : public HttpResponse {
    StreamDescriptor descriptor;
    const uint_least64_t length;
public:
    DescriptorResponse(uint responseCode, const StreamDescriptor descriptor)
            : HttpResponse(responseCode), descriptor(descriptor), length(0) {}

    DescriptorResponse(
            uint responseCode,
            const StreamDescriptor descriptor,
            const decltype(HEADERS) additionalHeaders,
            const uint_least64_t length
    ) : HttpResponse(responseCode), descriptor(descriptor), length(length) {}

    Socket &writeTo(Socket &socket) override;
};


#endif //TESTSUIT_DESCRIPTORRESPONSE_H
