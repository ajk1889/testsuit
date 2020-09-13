#ifndef TESTSUIT_HELPERS_H
#define TESTSUIT_HELPERS_H

#include <cstdio>
#include <vector>
#include <map>
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
using std::map;
using std::vector;

void *getCurl();

string currentDateTime();

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

class StreamDescriptor;

string readUntilMatch(StreamDescriptor &descriptor, const string &match, ULong maxLen = 8 * KB);

string readExact(StreamDescriptor &descriptor, long long nBytes);

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

template<typename Mappable>
void parseUrlEncodedPairs(const string &rawString, Mappable &outMap) {
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

void parseHttpHeader(const string &headerKeyValues, map<string, vector<string>> &headerMap);

bool startsWith(const char *search, const char *key);
#endif //TESTSUIT_HELPERS_H
