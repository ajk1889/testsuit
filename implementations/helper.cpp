#include "helpers.h"

string currentWorkingDir() {
    char cCurrentPath[FILENAME_MAX];
    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
        throw std::runtime_error("ERROR writing to socket");
    return cCurrentPath;
}