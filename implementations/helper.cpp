#include <iostream>
#include "file/StreamDescriptor.h"
#include "helpers.h"
#include "utils/ArrayJoiner.h"
#include "boost/algorithm/string.hpp"

string currentWorkingDir() {
    char cCurrentPath[FILENAME_MAX];
    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
        throw std::runtime_error("Couldn't get current working directory");
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

inline void fill(const char (&source)[BUFFER_SIZE + 1], string &destination, int bytesRead) {
    auto destinationLen = destination.length();
    if (destinationLen > bytesRead)
        memmove((void *) destination.c_str(), destination.c_str() + bytesRead, (destinationLen - bytesRead));
    while (bytesRead-- && destinationLen--)
        destination[destinationLen] = source[bytesRead];
}

bool matches(const ArrayJoiner &joined, const int offset, const string &key) {
    auto len = static_cast<int>(key.length());
    for (auto i = 0; i < len; ++i)
        if (joined[i + offset] != key[i])
            return false;
    return true;
}

int find(const char *hayStack, const string &key, const int hayStackLen, const string &lastFew) {
    if (hayStackLen == 0) return -1;
    const ArrayJoiner joined(lastFew, hayStack);
    for (int j = -static_cast<int>(lastFew.length()); j < hayStackLen; ++j) {
        if (matches(joined, j, key))
            return j + static_cast<int>(key.length());
    }
    return -1;
}

string readUntilMatch(StreamDescriptor &descriptor, const string &match, const ULong maxLen) {
    const auto lenMatch = match.length();
    if (lenMatch == 0)
        throw std::runtime_error("Parameter `match` should not be blank");
    char buffer[BUFFER_SIZE + 1];
    buffer[BUFFER_SIZE] = '\0';
    decltype(descriptor.read(buffer, BUFFER_SIZE)) bytesRead;
    string lastFew(lenMatch - 1, match[lenMatch - 1] - 1); // holds `lenMatch-1 bytes` which is != match[:-1]
    string data;
    while (data.length() < maxLen &&
           (bytesRead = descriptor.read(buffer, min(BUFFER_SIZE, maxLen - data.length()))) > 0) {
        auto terminationPoint = find(buffer, match, bytesRead, lastFew);
        if (terminationPoint == -1) {
            data.append(buffer, bytesRead);
            fill(buffer, lastFew, bytesRead);
        } else {
            data.append(buffer, terminationPoint);
            descriptor.unread(buffer + terminationPoint, bytesRead - terminationPoint);
            break;
        }
    }
    return std::move(data);
}

string readExact(StreamDescriptor &descriptor, long long nBytes) {
    string str;
    str.reserve(nBytes);
    char buffer[BUFFER_SIZE + 1];
    decltype(descriptor.read(buffer, min(BUFFER_SIZE, nBytes))) bytesRead;
    while (nBytes > 0 && (bytesRead = descriptor.read(buffer, min(BUFFER_SIZE, nBytes))) > -1) {
        buffer[bytesRead] = '\0';
        str.append(buffer);
        nBytes -= bytesRead;
    }
    return str;
}

void print(const string &tag, char *str, size_t lastIndex) {
    if (lastIndex == 0) {
        std::cout << tag << ": ||" << std::endl;
        return;
    }
    auto bkp = str[lastIndex];
    str[lastIndex] = '\0';
    std::cout << tag << ": |" << str << '|' << std::endl;
    str[lastIndex] = bkp;
}

void parseHttpHeader(const string &headerKeyValues, map<string, vector<string>> &headerMap) {
    int startPos = 0, separatorPos, endPos;
    while (true) {
        separatorPos = headerKeyValues.find(':', startPos);
        if (separatorPos == string::npos) break;
        endPos = headerKeyValues.find('\r', separatorPos);
        if (endPos == string::npos) {
            std::cerr << "Invalid headers: " << headerKeyValues << std::endl;
            throw std::runtime_error("Invalid headers");
        }
        auto key = headerKeyValues.substr(startPos, separatorPos - startPos);
        separatorPos += 2;
        auto value = headerKeyValues.substr(separatorPos, endPos - separatorPos);
        boost::trim(value);
        headerMap[key].push_back(value);
        startPos = endPos + 2;
    }
}

string currentDateTime() {
    time_t now = time(nullptr);
    tm *gmt = gmtime(&now);
    char buf[35];
    strftime(buf, sizeof(buf), "%a, %d %b %Y %T GMT", gmt);
    return buf;
}

bool startsWith(const char *search, const char *key) {
    size_t searchLen = strlen(search), keyLen = strlen(key);
    if (searchLen < keyLen) return false;
    while ((keyLen--) > 0) if (key[keyLen] != search[keyLen]) return false;
    return true;
}

auto preciseNow() -> decltype(std::chrono::high_resolution_clock::now()) {
    return std::chrono::high_resolution_clock::now();
}

void printChar(const string &tag, char *arr, uint32_t len) {
    char backup = arr[len];
    arr[len] = '\0';
    std::cout << tag << ": |" << arr << "|\ncount:" << len << std::endl;
    arr[len] = backup;
}
