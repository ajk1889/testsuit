#include <iostream>
#include "HttpRequest.h"
#include "../constants.h"
#include "../helpers.h"

ssize_t HttpRequest::read(char *buffer, uint N) const {
    auto copyLen = std::min(N, extraReadBytesLen - extraReadBytesOffset);
    if (copyLen > 0) {
        memcpy(buffer, extraReadBytes + extraReadBytesOffset, copyLen);
        extraReadBytesOffset += copyLen;
        buffer += copyLen;
        N -= copyLen;
    }
    return copyLen + socket->read(buffer, N);
}

inline void fill(const char *source, char (&dest)[3], int len) {
    auto offset = 3 - len;
    while (len--) dest[offset + len] = source[len];
}

string HttpRequest::extractRawHeaders() {
    char buffer[BUFSIZ + 1];
    buffer[BUFSIZ] = '\0';
    auto &client = *socket;
    auto n = client.read(buffer, BUFSIZ);
    char last3[] = {'0', '0', '0'}; // some random value other than \r\n\r\n
    string rawHeaders;
    while (n > -1 && rawHeaders.length() < MAX_HEADER_SIZE) {
        auto headerTermination = headerTerminationPoint(buffer, n, last3);
        if (headerTermination == -1) {
            buffer[n] = '\0';
            rawHeaders += buffer;
            fill(std::max(reinterpret_cast<char *>(buffer), buffer + n - 3), last3, n);
            n = client.read(buffer, BUFSIZ);
        } else {
            auto backup = buffer[headerTermination];
            buffer[headerTermination] = '\0';
            rawHeaders += buffer;
            buffer[headerTermination] = backup;
            memcpy(extraReadBytes, buffer + headerTermination, n - headerTermination);
            extraReadBytes[n - headerTermination] = '\0';
            break;
        }
    }
    if (rawHeaders.length() >= MAX_HEADER_SIZE)
        throw std::runtime_error("Error too long headers");
    return std::move(rawHeaders);
}

inline bool matchesCRLF(ArrayJoiner &joined, int offset) {
    return joined[offset] == '\r' && joined[offset + 1] == '\n' && joined[offset + 2] == '\r' &&
           joined[offset + 3] == '\n';
}

int HttpRequest::headerTerminationPoint(const char *buffer, int len, const char (&last3)[3]) {
    if (len == 0) return -1;
    ArrayJoiner joined(last3, buffer);
    for (int j = -3; j < len; ++j) {
        if (matchesCRLF(joined, j))
            return j + 4;
    }
    return -1;
}

void HttpRequest::extractGetParams(const string &basicString) {
    int start = 0, separator, end;
    while (true) {
        end = basicString.find('&', start);
        separator = basicString.find('=', start);
        if (end < 0) {
            if (separator >= 0) {
                GET[unEscape(basicString.substr(start, separator - start))] =
                        unEscape(basicString.substr(separator + 1));
            } else GET[unEscape(basicString.substr(start))];
            break;
        } else {
            if (separator >= 0) {
                GET[unEscape(basicString.substr(start, separator - start))] =
                        unEscape(basicString.substr(separator + 1, end - separator - 1));
            } else GET[unEscape(basicString.substr(start, end - start))];
            start = end + 1;
        }
    }
}

HttpRequest HttpRequest::from(const shared_ptr<Socket> &client) {
    auto req = HttpRequest(client);
    auto rawHeaders = req.extractRawHeaders();
    auto headersLen = rawHeaders.length();

    // finding HTTP Method
    size_t leftFlagPos = 0, rightFlagPos = rawHeaders.find(' ');
    if (rightFlagPos < 3 || rightFlagPos > 6)
        throw std::runtime_error("Invalid request method\n" + rawHeaders);
    req.requestType = rawHeaders.substr(leftFlagPos, rightFlagPos - leftFlagPos);
    leftFlagPos = rightFlagPos + 1;

    // finding request path
    if (leftFlagPos >= headersLen)
        throw std::runtime_error("Invalid headers\n" + rawHeaders);
    rightFlagPos = rawHeaders.find(' ', leftFlagPos);
    req.path = rawHeaders.substr(leftFlagPos, rightFlagPos - leftFlagPos);
    auto queryStartIndex = req.path.find('?');
    if (queryStartIndex != -1) {
        req.extractGetParams(req.path.substr(queryStartIndex + 1));
        req.path = unEscape(req.path, queryStartIndex);
    } else req.path = unEscape(req.path, req.path.length());
    leftFlagPos = rightFlagPos + 1;

    // finding HTTP protocol version
    if (leftFlagPos >= headersLen)
        throw std::runtime_error("Invalid headers\n" + rawHeaders);
    rightFlagPos = rawHeaders.find('\r', leftFlagPos);
    req.httpVersion = rawHeaders.substr(leftFlagPos, rightFlagPos - leftFlagPos);

    // setting HTTP-Headers
    leftFlagPos = rightFlagPos + 2;
    if (leftFlagPos >= headersLen)
        throw std::runtime_error("Incomplete headers\n" + rawHeaders);
    req.extractHeaderKeyValues(rawHeaders.substr(leftFlagPos));
    return req;
}

void HttpRequest::extractHeaderKeyValues(const string &headerKeyValues) {
    int startPos = 0, separatorPos, endPos;
    while (true) {
        separatorPos = headerKeyValues.find(':', startPos);
        if (separatorPos == -1) break;
        endPos = headerKeyValues.find('\r', separatorPos);
        auto key = headerKeyValues.substr(startPos, separatorPos - startPos);
        separatorPos += 2;
        auto value = headerKeyValues.substr(separatorPos, endPos - separatorPos);
        HEADERS[key].push_back(value);
        startPos = endPos + 2;
    }
}
