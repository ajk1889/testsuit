#ifndef TESTSUIT_HELPERS_H
#define TESTSUIT_HELPERS_H

#include <cstdio>
#include <string>
#include <curl/curl.h>

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else

#include <unistd.h>
#include <stdexcept>

#define GetCurrentDir getcwd
#endif

using std::string;

void *getCurl();

template<typename A, typename B>
inline A min(A a, B b) {
    return a < b ? a : b;
}

template<typename A, typename B>
inline A max(A a, B b) {
    return a > b ? a : b;
}

string currentWorkingDir();

inline size_t len(const string &);

constexpr size_t len(char);

template<typename StrOrChar1, typename StrOrChar2>
void replaceAll(string &base, const StrOrChar1 &searchKey, const StrOrChar2 &replaceKey) {
    size_t replaceLen = len(replaceKey);
    size_t searchLen = len(searchKey);
    size_t index = 0;
    while ((index = base.find(searchKey, index)) != std::string::npos) {
        base.replace(index, searchLen, replaceKey);
        index += replaceLen;
    }
}

string urlDecode(string &escaped, size_t length = 0);

string urlDecode(const string &escaped, size_t length = 0);

#endif //TESTSUIT_HELPERS_H
