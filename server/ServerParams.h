#ifndef TESTSUIT_SERVERPARAMS_H
#define TESTSUIT_SERVERPARAMS_H

#include <iostream>
#include <set>
#include <string>
#include <limits>
#include <filesystem>
#include "../lib/json.hpp"
#include "../implementations/helpers.h"
#include "../implementations/constants.h"

using std::set;
using std::ostream;
using std::string;
using json = nlohmann::json;

struct ServerParams {
    uint32_t pingMs = 0;
    uint32_t maxDownloadSpeed = UINT32_MAX;
    uint32_t maxUploadSpeed = UINT32_MAX;
    uint32_t parallelConnections = 10;
    u_short port = 1234;
    string tempDir = std::filesystem::temp_directory_path().string();
    bool loggingAllowed = true;
    string urlMapFile;

    map<string, string> additionalKwargs;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
            ServerParams, pingMs, maxDownloadSpeed,
            maxUploadSpeed, parallelConnections, port,
            tempDir, loggingAllowed, urlMapFile,
            additionalKwargs
    )
};
#endif //TESTSUIT_SERVERPARAMS_H
