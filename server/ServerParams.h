#ifndef TESTSUIT_SERVERPARAMS_H
#define TESTSUIT_SERVERPARAMS_H

#include <iostream>
#include <set>
#include <string>
#include <limits>
#include "../implementations/helpers.h"
#include "../implementations/constants.h"

using std::set;
using std::ostream;
using std::string;
struct ServerParams {
    uint32_t pingMs = 0;
    uint32_t maxDownloadSpeed = UINT32_MAX;
    uint32_t maxUploadSpeed = UINT32_MAX;
    uint32_t parallelConnections = 10;
    u_short port = 1234;
    string sharedPath = currentWorkingDir();
    set<string> ignoredPaths;
    string uploadsPath = sharedPath;
    ostream &sharedTextOutput = std::cout;
    string path123 = "/123.bin";
    uint64_t size123 = 100 * MB;
    bool preferFileOver123 = false;
    string cookie;
    set<string> cookieRequiredPaths; // if empty, all paths will require cookie
    string cookieSettingPath = "/set-cookie";
    bool contentLengthMode = false;
    bool noLengthMode = false;
    bool loggingAllowed = true;
    string urlMapFile;
};
#endif //TESTSUIT_SERVERPARAMS_H
