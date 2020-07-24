//
// Created by ajk on 24/07/20.
//

#ifndef TESTSUIT_SERVERPARAMS_H
#define TESTSUIT_SERVERPARAMS_H

#include <iostream>
#include <set>
#include <string>
#include "../implementations/constants.h"

using std::set;
using std::ostream;
using std::string;
struct ServerParams {
    uint pingMs;
    uint maxDownloadSpeed;
    uint maxUploadSpeed;
    uint parallelConnections;
    u_short port;
    string sharedPath;
    set<string> ignoredPaths;
    string uploadsPath;
    ostream sharedTextOutput;
    string path123;
    uint_least64_t size123 = 100 * MB;
    bool preferFileOver123 = false;
    string cookie;
    set<string> cookieRequiredPaths; // if empty, all paths will require cookie
    string cookieSettingPath = "/set-cookie";
};
#endif //TESTSUIT_SERVERPARAMS_H
