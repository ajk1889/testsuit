#ifndef TESTSUIT_DESCRIPTORRESPONSE_H
#define TESTSUIT_DESCRIPTORRESPONSE_H


#include "HttpResponse.h"

class DescriptorResponse : public HttpResponse {
    StreamDescriptor descriptor;
    const uint_least64_t length;
public:
    DescriptorResponse(uint responseCode, const StreamDescriptor &descriptor)
            : HttpResponse(responseCode), descriptor(descriptor), length(0) {}

    DescriptorResponse(
            uint responseCode,
            StreamDescriptor &descriptor,
            const decltype(HEADERS) &additionalHeaders,
            const uint_least64_t length
    ) : HttpResponse(responseCode, additionalHeaders), descriptor(descriptor), length(length) {
        if (length != 0) HEADERS["Content-Length"].push_back(std::to_string(length));
    }

    Socket &writeTo(Socket &socket) override;
};


#endif //TESTSUIT_DESCRIPTORRESPONSE_H
