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
#include <iostream>
#include <stdexcept>
#include "constants.h"

#define GetCurrentDir getcwd
#endif

using std::string;

void *getCurl();

template<typename A, typename B>
inline auto min(const A a, const B b) -> decltype(a < b ? a : b) {
    return a < b ? a : b;
}

template<typename A, typename B>
inline auto max(const A a, const B b) -> decltype(a > b ? a : b) {
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

class Socket;

string readUntilMatch(Socket &socket, const string &match, ULong maxLen = 8 * KB);

string readExact(Socket &socket, long long nBytes);

int find(const char *hayStack, const string &key, const int hayStackLen, const string &lastFew);

void print(const string &tag, char *str, size_t lastIndex);

template<typename A>
void print(A a) {
    std::cout << a << std::endl;
}

template<typename A, typename ...B>
void print(A a, B ...b) {
    std::cout << a << ' ';
    print(b...);
}

#endif //TESTSUIT_HELPERS_H
