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

string unEscape(const string &escaped, size_t len) {
    return curl_easy_unescape(getCurl(), escaped.c_str(), std::min(escaped.length(), len), nullptr);
}