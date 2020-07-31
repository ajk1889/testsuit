#include <iostream>
#include <sstream>
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
    char buffer[BUFFER_SIZE + 1];
    buffer[BUFFER_SIZE] = '\0';
    auto &client = *socket;
    decltype(client.read(buffer, BUFFER_SIZE)) bytesRead;
    char last3[] = {'0', '0', '0'}; // some random value other than \r\n\r\n
    string rawHeaders;
    while ((bytesRead = client.read(buffer, BUFFER_SIZE)) > -1 && rawHeaders.length() < MAX_HEADER_SIZE) {
        auto headerTermination = headerTerminationPoint(buffer, bytesRead, last3);
        if (headerTermination == -1) {
            buffer[bytesRead] = '\0';
            rawHeaders += buffer;
            fill(std::max(reinterpret_cast<char *>(buffer), buffer + bytesRead - 3), last3, bytesRead);
        } else {
            auto backup = buffer[headerTermination];
            buffer[headerTermination] = '\0';
            rawHeaders += buffer;
            buffer[headerTermination] = backup;
            memcpy(extraReadBytes, buffer + headerTermination, bytesRead - headerTermination);
            extraReadBytesLen = bytesRead - headerTermination;
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

string readStringFromRequest(const HttpRequest &request, long long nBytes) {
    string str;
    str.reserve(nBytes);
    char buffer[BUFFER_SIZE + 1]{};
    decltype(request.read(buffer, min(BUFFER_SIZE, nBytes))) bytesRead;
    while ((bytesRead = request.read(buffer, min(BUFFER_SIZE, nBytes))) > -1 && nBytes > 0) {
        buffer[bytesRead] = '\0';
        str.append(buffer);
        nBytes -= bytesRead;
    }
    return str;
}

void parseUrlEncodedPairs(const string &basicString, map<string, string> &outMap) {
    int start = 0, separator, end;
    while (true) {
        end = basicString.find('&', start);
        separator = basicString.find('=', start);
        if (end == string::npos) {
            if (separator != string::npos) {
                outMap[urlDecode(basicString.substr(start, separator - start))] =
                        urlDecode(basicString.substr(separator + 1));
            } else outMap[urlDecode(basicString.substr(start))];
            break;
        } else {
            if (separator != string::npos) {
                outMap[urlDecode(basicString.substr(start, separator - start))] =
                        urlDecode(basicString.substr(separator + 1, end - separator - 1));
            } else outMap[urlDecode(basicString.substr(start, end - start))];
            start = end + 1;
        }
    }
}

void extractRequestDataConditionally(HttpRequest &httpRequest) {
    auto searchResult = httpRequest.HEADERS.find("Content-Length");
    if (searchResult == httpRequest.HEADERS.cend()) return;
    try {
        auto contentLength = std::stoll(searchResult->second[0]);
        if (contentLength == 0) return;
        if (contentLength < 1 * MB) {
            searchResult = httpRequest.HEADERS.find("Content-Type");
            if (searchResult != httpRequest.HEADERS.cend() &&
                searchResult->second[0] == "application/x-www-form-urlencoded") {
                string postData = readStringFromRequest(httpRequest, contentLength);
                parseUrlEncodedPairs(postData, httpRequest.POST);
            }
        } else throw std::runtime_error("Post data above 1 MB is not supported now");
    } catch (...) {}
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
    if (rightFlagPos == string::npos)
        throw std::runtime_error("Invalid headers\n" + rawHeaders);
    req.path = rawHeaders.substr(leftFlagPos, rightFlagPos - leftFlagPos);
    auto queryStartIndex = req.path.find('?');
    if (queryStartIndex != string::npos) {
        parseUrlEncodedPairs(req.path.substr(queryStartIndex + 1), req.GET);
        req.path = urlDecode(req.path, queryStartIndex);
    } else req.path = urlDecode(req.path);
    leftFlagPos = rightFlagPos + 1;

    // finding HTTP protocol version
    if (leftFlagPos >= headersLen)
        throw std::runtime_error("Invalid headers\n" + rawHeaders);
    rightFlagPos = rawHeaders.find('\r', leftFlagPos);
    if (rightFlagPos == string::npos)
        throw std::runtime_error("Invalid headers\n" + rawHeaders);
    req.httpVersion = rawHeaders.substr(leftFlagPos, rightFlagPos - leftFlagPos);

    // setting HTTP-Headers
    leftFlagPos = rightFlagPos + 2;
    if (leftFlagPos >= headersLen)
        throw std::runtime_error("Incomplete headers\n" + rawHeaders);
    req.extractHeaderKeyValues(rawHeaders.substr(leftFlagPos));

    extractRequestDataConditionally(req);
    return req;
}

void HttpRequest::extractHeaderKeyValues(const string &headerKeyValues) {
    int startPos = 0, separatorPos, endPos;
    while (true) {
        separatorPos = headerKeyValues.find(':', startPos);
        if (separatorPos == string::npos) break;
        endPos = headerKeyValues.find('\r', separatorPos);
        if (endPos == string::npos)
            throw std::runtime_error("Invalid headers\n" + headerKeyValues);
        auto key = headerKeyValues.substr(startPos, separatorPos - startPos);
        separatorPos += 2;
        auto value = headerKeyValues.substr(separatorPos, endPos - separatorPos);
        HEADERS[key].push_back(value);
        startPos = endPos + 2;
    }
}
