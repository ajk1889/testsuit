#include <iostream>
#include "networking/Socket.h"
#include "helpers.h"
#include "ArrayJoiner.h"

string currentWorkingDir() {
    char cCurrentPath[FILENAME_MAX];
    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
        throw std::runtime_error("ERROR writing to socket");
    return cCurrentPath;
}

void *getCurl() {
    static void *curl = nullptr;
    if (curl == nullptr)
        return curl = curl_easy_init();
    else return curl;
}

inline size_t len(const string &s) {
    return s.length();
}

constexpr size_t len(char) {
    return 1;
}

string urlDecode(string &escaped, size_t length) {
    replaceAll(escaped, '+', "%20");
    return curl_easy_unescape(getCurl(), escaped.c_str(), std::min(escaped.length(), length), nullptr);
}

string urlDecode(const string &escaped, size_t length) {
    auto copied = escaped;
    return urlDecode(copied, length);
}

inline void fill(const char *source, string &destination, int len) {
    auto offset = destination.length() - len;
    while (len--) destination[offset + len] = source[len];
}

bool matches(ArrayJoiner &joined, const int offset, const string &key) {
    auto len = static_cast<int>(key.length());
    for (auto i = 0; i < len; ++i) {
        if (joined[i + offset] != key[i])
            return false;
    }
    return true;
}

int find(const char *hayStack, const string &key, const int hayStackLen, const string &lastFew) {
    if (hayStackLen == 0) return -1;
    ArrayJoiner joined(lastFew, hayStack);
    for (int j = -static_cast<int>(lastFew.length()); j < hayStackLen; ++j) {
        if (matches(joined, j, key))
            return j + lastFew.length() + 1;
    }
    return -1;
}

string readUntilMatch(Socket &socket, const string &match, const ULong maxLen) {
    const auto lenMatch = match.length();
    if (lenMatch == 0)
        throw std::runtime_error("Parameter `match` should not be blank");
    char buffer[BUFFER_SIZE + 1];
    buffer[BUFFER_SIZE] = '\0';
    decltype(socket.read(buffer, BUFFER_SIZE)) bytesRead;
    string lastFew(lenMatch - 1, match[lenMatch - 1] - 1); // holds `lenMatch-1 bytes` which is != match[:-1]
    string rawHeaders;
    while (rawHeaders.length() < maxLen && (bytesRead = socket.read(buffer, BUFFER_SIZE)) > -1) {
        auto terminationPoint = find(buffer, match, bytesRead, lastFew);
        if (terminationPoint == -1) {
            buffer[bytesRead] = '\0';
            rawHeaders += buffer;
            fill(std::max(reinterpret_cast<char *>(buffer), buffer + bytesRead - lenMatch), lastFew, bytesRead);
        } else {
            auto backup = buffer[terminationPoint];
            buffer[terminationPoint] = '\0';
            rawHeaders += buffer;
            buffer[terminationPoint] = backup;
            socket.unread(buffer + terminationPoint, bytesRead - terminationPoint);
            break;
        }
    }
    return std::move(rawHeaders);
}

string readExact(Socket &socket, long long nBytes) {
    string str;
    str.reserve(nBytes);
    char buffer[BUFFER_SIZE + 1]{};
    decltype(socket.read(buffer, min(BUFFER_SIZE, nBytes))) bytesRead;
    while (nBytes > 0 && (bytesRead = socket.read(buffer, min(BUFFER_SIZE, nBytes))) > -1) {
        buffer[bytesRead] = '\0';
        str.append(buffer);
        nBytes -= bytesRead;
    }
    return str;
}

void print(char *str, size_t lastIndex) {
    auto bkp = str[lastIndex];
    str[lastIndex] = '\0';
    std::cout << str << std::endl;
    str[lastIndex] = bkp;
}