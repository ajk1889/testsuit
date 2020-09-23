#ifndef TESTSUIT_SERVERPARAMS_H
#define TESTSUIT_SERVERPARAMS_H

#include <iostream>
#include <set>
#include <string>
#include <limits>
#include <filesystem>
#include <fstream>
#include <boost/algorithm/string/trim.hpp>
#include "json/json.hpp"
#include "../implementations/helpers.h"
#include "../implementations/constants.h"
#include "../implementations/utils/Process.h"

using std::set;
using std::ostream;
using std::string;
using std::ifstream;
using json = nlohmann::json;
using std::chrono_literals::operator ""ms;

class ServerParams {
    friend class StreamDescriptor;

    decltype(33ms) timeDiff = 33ms;
    int32_t writeBytesPerTimeDiff = 100;

public:
    uint32_t pingMs = 0;
    uint32_t maxDownloadSpeed = UINT32_MAX;
    uint32_t maxUploadSpeed = UINT32_MAX;
    uint32_t parallelConnections = 10;
    u_short port = 1234;
    string tempDir;
    bool loggingAllowed = true;
    string urlMapFile = "/home/ajk/CLionProjects/testsuit/urlMap.json";

    map<string, vector<string>> urlMap;
    map<string, string> additionalKwargs;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
            ServerParams, pingMs, maxDownloadSpeed,
            maxUploadSpeed, parallelConnections, port,
            tempDir, loggingAllowed, urlMapFile,
            additionalKwargs
    )

    map<string, string> allowedParams = {
            {"maxdspeed", "Maximum download speed, in kb/s"},
            {"maxuspeed", "Maximum upload speed, in kb/s"},
            {"logging",   "Allow request logging"},
            {"temp-dir",  "Folder to which temporary files should be written"},
            {"ping",      "The time (in ms) for which server should idle wait after receiving a request"},
            {"urlmap",    "absolute path to the URL Mapping file to use"}
    };

    void initializeUrlMap(string urlMapFilePath);

    void initializeFrom(int argc, char *argv[]);
};

extern ServerParams params;
#endif //TESTSUIT_SERVERPARAMS_H
