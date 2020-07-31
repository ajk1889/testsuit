#include <iostream>
#include "helpers.h"

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
