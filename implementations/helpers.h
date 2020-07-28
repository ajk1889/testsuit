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

string currentWorkingDir();

string unEscape(const string &escaped, size_t length = 0);
#endif //TESTSUIT_HELPERS_H
