#include "HttpRequest.h"
#include "boost/algorithm/string.hpp"

void ignoreStreamUntil(Socket &socket, const string &boundary) {
    auto end = readUntilMatch(socket, boundary, 1 * KB);
    while (end.rfind(boundary) == string::npos)
        end = readUntilMatch(socket, boundary, 1 * KB);
}

void parseMultiPartFormData(const HttpRequest &request, const string &boundary, json &outMap) {
    auto &socket = *request.socket;
    ignoreStreamUntil(socket, "--" + boundary); // ignore until beginning boundary
    ignoreStreamUntil(socket, "\r\n"); // ignore until boundary's whitespaces

    auto realBoundary = "\r\n--" + boundary;
    while (true) {
        const auto rawItemHeader = readUntilMatch(socket, "\r\n\r\n");
        map<string, vector<string>> parsedItemHeader;
        parseHttpHeader(rawItemHeader, parsedItemHeader);
        auto result = parsedItemHeader.find(ContentDisposition::KEY);
        if (result != parsedItemHeader.cend()) {
            const ContentDisposition disposition = result->second[0];
            if (outMap.find(disposition.name) == outMap.end())
                outMap[disposition.name] = {};
            auto &thisItem = outMap[disposition.name];
            if (disposition.type == ContentDisposition::TYPE_FORM_DATA) {
                thisItem.push_back({
                                           {"headers", parsedItemHeader},
                                           {"data",    FileOrString::readFrom(socket, realBoundary)}
                                   });
                char next2bytes[2];
                socket.read(next2bytes);
                if (next2bytes[0] == '-' && next2bytes[1] == '-')
                    break; // no more form data
                else socket.unread(next2bytes, 2);
                readUntilMatch(socket, "\r\n"); // boundary's whitespaces, ignorable
            } else throw std::runtime_error("Unsupported content type");
        } else {
            std::cerr << rawItemHeader << std::endl;
            throw std::runtime_error("Item without Content-Disposition");
        }
    }
}

void extractRequestDataConditionally(HttpRequest &req) {
    auto contentTypeIter = req.HEADERS.find("Content-Type");
    if (contentTypeIter != req.HEADERS.cend()) {
        auto contentType = contentTypeIter->second[0];
        if (contentType == "application/x-www-form-urlencoded") {
            auto contentLengthIter = req.HEADERS.find("Content-Length");
            if (contentLengthIter == req.HEADERS.cend()) return;
            auto contentLength = std::stoll(contentLengthIter->second[0]);
            if (contentLength == 0) return;
            if (contentLength >= 2 * MB)
                throw std::runtime_error("POST should be below 2 MB for application/x-www-form-urlencoded data");
            string postData = readExact(*req.socket, contentLength);
            parseUrlEncodedPairs(postData, req.POST);
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
        } else {
            auto contentLengthIter = req.HEADERS.find("Content-Length");
            if (contentLengthIter == req.HEADERS.cend()) return;
            auto contentLength = std::stoll(contentLengthIter->second[0]);
            if (contentLength == 0) return;
            req.POST = FileOrString::readFrom(*req.socket, contentLength);
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
