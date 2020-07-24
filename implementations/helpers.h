#ifndef TESTSUIT_HELPERS_H
#define TESTSUIT_HELPERS_H

#include <cstdio>  /* defines FILENAME_MAX */
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

string currentWorkingDir() {
    char cCurrentPath[FILENAME_MAX];
    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
        throw std::runtime_error("ERROR writing to socket");
    return cCurrentPath;
}

#endif //TESTSUIT_HELPERS_H
