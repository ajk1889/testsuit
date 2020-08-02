#include <iostream>
#include "HttpRequest.h"
#include "boost/algorithm/string.hpp"
#include "http/ContentDisposition.h"

void parseUrlEncodedPairs(const string &rawString, map<string, string> &outMap) {
    int start = 0, separator, end;
    while (true) {
        end = rawString.find('&', start);
        separator = rawString.find('=', start);
        if (end == string::npos) {
            if (separator != string::npos) {
                outMap[urlDecode(rawString.substr(start, separator - start))] =
                        urlDecode(rawString.substr(separator + 1));
            } else outMap[urlDecode(rawString.substr(start))];
            break;
        } else {
            if (separator != string::npos) {
                outMap[urlDecode(rawString.substr(start, separator - start))] =
                        urlDecode(rawString.substr(separator + 1, end - separator - 1));
            } else outMap[urlDecode(rawString.substr(start, end - start))];
            start = end + 1;
        }
    }
}

void parseHttpHeader(const string &headerKeyValues, map<string, vector<string>> &headerMap) {
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
        boost::trim(value);
        headerMap[key].push_back(value);
        startPos = endPos + 2;
    }
}

void parseMultiPartFormData(const HttpRequest &request, const string &boundary, map<string, string> &outMap) {
    auto &socket = *request.socket;
    readUntilMatch(socket, "--" + boundary); // beginning boundary, ignorable
    readUntilMatch(socket, "\r\n"); // boundary's whitespaces, ignorable
    auto realBoundary = "\r\n--" + boundary;
    while (true) {
        const auto rawItemHeader = readUntilMatch(socket, "\r\n\r\n");
        map<string, vector<string>> parsedItemHeader;
        parseHttpHeader(rawItemHeader, parsedItemHeader);
        auto result = parsedItemHeader.find(ContentDisposition::KEY);
        if (result != parsedItemHeader.cend()) {
            const ContentDisposition disposition = result->second[0];
            if (disposition.type == ContentDisposition::TYPE_FORM_DATA) {
                // TODO check whether the data is a file
                auto value = readUntilMatch(socket, realBoundary, 10 * KB); // TODO change max read post data size
                value = value.substr(0, value.find(realBoundary));
                outMap[disposition.name] = value;
                char next2bytes[2];
                socket.read(next2bytes);
                if (next2bytes[0] == '-' && next2bytes[1] == '-')
                    break; // no more form data
                else socket.unread(next2bytes, 2);
                readUntilMatch(socket, "\r\n"); // boundary's whitespaces, ignorable
            }
        } else throw std::runtime_error("Item without Content-Disposition");
    }
}

void extractRequestDataConditionally(HttpRequest &req) {
    auto contentTypeIter = req.HEADERS.find("Content-Type");
    if (contentTypeIter != req.HEADERS.cend()) {
        auto contentType = contentTypeIter->second[0];
        if (contentType == "application/x-www-form-urlencoded") {
            auto contentLengthIter = req.HEADERS.find("Content-Length");
            if (contentLengthIter == req.HEADERS.cend()) return;
            try {
                auto contentLength = std::stoll(contentLengthIter->second[0]);
                if (contentLength == 0) return;
                if (contentLength >= 2 * MB)
                    throw std::runtime_error("POST should be below 2 MB for application/x-www-form-urlencoded data");
                string postData = readExact(*req.socket, contentLength);
                parseUrlEncodedPairs(postData, req.POST);
            } catch (...) {
                throw std::runtime_error("Invalid content length value " + contentLengthIter->second[0]);
            }
        } else if (contentType.find("multipart/form-data;") != string::npos) {
            constexpr auto lenSearchKey = 11; // len('; boundary=')
            auto boundaryStart = contentType.find("; boundary=");
            if (boundaryStart == string::npos)
                throw std::runtime_error("Invalid boundary for multipart/form-data " + contentType);
            boundaryStart += lenSearchKey;
            auto boundary = contentType.substr(boundaryStart);
            if (boundary[0] == '"')
                boundary = boundary.substr(1, boundary.length() - 1);
            else boost::trim(boundary);
            parseMultiPartFormData(req, boundary, req.POST);
        }
    }
}


HttpRequest HttpRequest::from(const shared_ptr<Socket> &client) {
    auto req = HttpRequest(client);
    const auto rawHeaders = readUntilMatch(*client, "\r\n\r\n", MAX_HEADER_SIZE);
    const auto headersLen = rawHeaders.length();

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
    parseHttpHeader(rawHeaders.substr(leftFlagPos), req.HEADERS);

    extractRequestDataConditionally(req);
    return req;
}
