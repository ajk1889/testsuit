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

    void initializeFrom(int argc, char *argv[]) {
        for (int i = 1; i < argc; ++i) {
            if (strstr(argv[i], "--pingMs=") != nullptr) {
                pingMs = atoi(index(argv[i], '=') + 1);
                std::cout << "Ping: " << pingMs << std::endl;
            } else if (strstr(argv[i], "--maxDownloadSpeed=") != nullptr) {
                maxDownloadSpeed = atoi(index(argv[i], '=') + 1);
                std::cout << "Max download speed: " << maxDownloadSpeed << std::endl;
            } else if (strstr(argv[i], "--maxUploadSpeed=") != nullptr) {
                maxUploadSpeed = atoi(index(argv[i], '=') + 1);
                std::cout << "Max upload speed: " << maxUploadSpeed << std::endl;
            } else if (strstr(argv[i], "--port=") != nullptr) {
                port = atoi(index(argv[i], '=') + 1);
                std::cout << "Port: " << port << std::endl;
            } else if (strstr(argv[i], "--parallelConnections=") != nullptr) {
                parallelConnections = atoi(index(argv[i], '=') + 1);
                std::cout << "Max parallel connections: " << parallelConnections << std::endl;
            } else if (strstr(argv[i], "--loggingAllowed=") != nullptr) {
                loggingAllowed = atoi(index(argv[i], '=') + 1);
                std::cout << "Logging allowed: " << loggingAllowed << std::endl;
            } else if (strstr(argv[i], "--urlMapFile=") != nullptr) {
                urlMapFile = index(argv[i], '=') + 1;
                std::cout << "Url mapping file: " << urlMapFile << std::endl;
            } else if (strstr(argv[i], "--tempDir=") != nullptr) {
                tempDir = index(argv[i], '=') + 1;
                std::cout << "Temp files directory: " << tempDir << std::endl;
            }
        }
    }
};
#endif //TESTSUIT_SERVERPARAMS_H
