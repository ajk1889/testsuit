#ifndef TESTSUIT_SERVERPARAMS_H
#define TESTSUIT_SERVERPARAMS_H

#include <iostream>
#include <set>
#include <string>
#include <limits>
#include <fstream>
#include <regex>
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
class Server;

class ServerParams {
    uint32_t maxDownloadSpeed;
    uint32_t minDownloadSpeed;
    uint32_t maxUploadSpeed;
    uint32_t minUploadSpeed;
    uint32_t speedChangeInterval;

    string tempDir;
    constexpr static auto TIME_DIFF_MS = 33;

    friend class Server;
public:
    uint32_t pingMs = 0;
    uint32_t parallelConnections = 10;
    u_short port = 1234;
    bool loggingAllowed = true;
    bool disableStdin = false;
    string urlMapFile = thisExecutablePath() + "/urlMap.json";

    constexpr static auto timeDiff = std::chrono::milliseconds(TIME_DIFF_MS);
    int32_t writeBytesPerTimeDiff;
    int32_t readBytesPerTimeDiff;

    vector<UrlMapObject> urlMap;
    map<string, string> additionalKwargs;

    [[nodiscard]] uint32_t getMaxDownloadSpeed() const { return maxDownloadSpeed; };

    const string &getTempDir() const {
        if (!exists(tempDir) && system(("mkdir '" + tempDir + "'").c_str()))
            throw std::runtime_error("Could not create temp directory " + tempDir);
        return tempDir;
    }

    [[nodiscard]] uint32_t getMaxUploadSpeed() const { return maxUploadSpeed; }

    void setMaxUploadSpeed(uint32_t speed) {
        maxUploadSpeed = speed;
        readBytesPerTimeDiff = 1024 * speed * TIME_DIFF_MS / 1000;
    }
    void setMaxDownloadSpeed(uint32_t speed) {
        maxDownloadSpeed = speed;
        writeBytesPerTimeDiff = 1024 * speed * TIME_DIFF_MS / 1000;
    };

    void setVaryingUploadSpeed(string command) {
        std::regex expr(" *(\\d+) *- *(\\d+) +in +(\\d+) *(second|sec)?s? *");
        std::smatch sm;
        if (std::regex_match(command, sm, expr)) {
            maxDownloadSpeed = std::stol(sm[1]);
            minDownloadSpeed = std::stol(sm[2]);
            speedChangeInterval = std::stol(sm[3]);
            std::cout << "Download speed will randomly vary between " << maxDownloadSpeed 
                      << "KB/s and " << minDownloadSpeed << "KB/s every "
                      << speedChangeInterval << " seconds." << std::endl;
        } else {
            std::cout << "Invalid syntax, follow format specified below\n"
                      << "random-dspeed=10-30 in 4 s\n"
                      << "This command will vary download speed randomly between 10 and 30 in every 4 seconds"
                      << std::endl;
        }
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
            ServerParams, pingMs, maxDownloadSpeed,
            maxUploadSpeed, parallelConnections, port,
            tempDir, loggingAllowed, urlMapFile,
            additionalKwargs, disableStdin
    )

    map<string, string> allowedParams = {
            {"maxdspeed", "Maximum download speed, in KB/s"},
            {"maxuspeed", "Maximum upload speed, in KB/s"},
            {"random-dspeed", "Format `A-B in N s`. Add randomness to download speed in a range A KB/s to B KB/s. Download speed will vary in N second interval"},
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
