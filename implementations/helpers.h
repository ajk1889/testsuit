#ifndef TESTSUIT_HELPERS_H
#define TESTSUIT_HELPERS_H

#include <cstdio>
#include <string>

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else

#include <unistd.h>
#include <stdexcept>

#define GetCurrentDir getcwd
#endif

using std::string;

string currentWorkingDir();

#endif //TESTSUIT_HELPERS_H
