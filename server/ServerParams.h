#ifndef TESTSUIT_SERVERPARAMS_H
#define TESTSUIT_SERVERPARAMS_H

#include <iostream>
#include <set>
#include <string>
#include <limits>
#include <experimental/filesystem>
#include <fstream>
#include <boost/algorithm/string/trim.hpp>
#include "json/json.hpp"
#include "../implementations/helpers.h"
#include "../implementations/constants.h"
#include "../implementations/utils/Process.h"
#include "../implementations/utils/UrlMapObject.h"

using std::set;
using std::ostream;
using std::string;
using std::ifstream;
using json = nlohmann::json;
using std::chrono_literals::operator ""ms;

class ServerParams {
    uint32_t maxDownloadSpeed;
    uint32_t maxUploadSpeed;
    constexpr static auto TIME_DIFF_MS = 33;
public:
    uint32_t pingMs = 0;
    uint32_t parallelConnections = 10;
    u_short port = 1234;
    string tempDir;
    bool loggingAllowed = true;
    string urlMapFile = currentWorkingDir() + "/urlMap.json";

    constexpr static auto timeDiff = std::chrono::milliseconds(TIME_DIFF_MS);
    int32_t writeBytesPerTimeDiff;
    int32_t readBytesPerTimeDiff;

    vector<UrlMapObject> urlMap;
    map<string, string> additionalKwargs;

    [[nodiscard]] uint32_t getMaxDownloadSpeed() const { return maxDownloadSpeed; };

    void setMaxDownloadSpeed(uint32_t speed) {
        maxDownloadSpeed = speed;
        writeBytesPerTimeDiff = 1024 * speed * TIME_DIFF_MS / 1000;
    };

    [[nodiscard]] uint32_t getMaxUploadSpeed() const { return maxUploadSpeed; }

    void setMaxUploadSpeed(uint32_t speed) {
        maxUploadSpeed = speed;
        readBytesPerTimeDiff = 1024 * speed * TIME_DIFF_MS / 1000;
    }

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
