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
class StreamDescriptor;

class ServerParams {
    uint32_t maxDownloadSpeed;
    uint32_t maxUploadSpeed;
    constexpr static auto TIME_DIFF_MS = 33;
    std::pair<uint32_t, uint32_t> downloadSpeedRange;
    std::pair<uint32_t, uint32_t> uploadSpeedRange;
    constexpr static auto timeDiff = std::chrono::milliseconds(TIME_DIFF_MS);
    int32_t writeBytesPerTimeDiff;
    int32_t readBytesPerTimeDiff;

    uint32_t downloadSpeedChangeInterval;
    decltype(preciseNow()) lastDownloadSpeedChangedTime = preciseNow();

    string tempDir;

    friend class Server;
    friend class StreamDescriptor;
public:
    uint32_t pingMs = 0;
    uint32_t parallelConnections = 10;
    u_short port = 1234;
    bool loggingAllowed = true;
    bool disableStdin = false;
    string urlMapFile = thisExecutablePath() + "/urlMap.json";

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
        std::regex expr(" *(\\d+) *- *(\\d+) +in +([0-9.]+) *(ms)? *");
        std::smatch sm;
        if (std::regex_match(command, sm, expr)) {
            downloadSpeedRange = std::pair<uint32_t, uint32_t>(std::stol(sm[1]), std::stol(sm[2]));
            downloadSpeedChangeInterval = std::stol(sm[3]);
            std::cout << "Download speed will randomly vary between " << downloadSpeedRange.first 
                      << "KB/s and " << downloadSpeedRange.second << "KB/s every "
                      << downloadSpeedChangeInterval << " milliseconds." << std::endl;
        } else {
            std::cout << "Invalid syntax, follow format specified below\n"
                      << "random-dspeed=10-30 in 4000\n"
                      << "This command will vary download speed randomly between 10 and 30 in every 4000 milliseconds"
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
