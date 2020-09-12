#ifndef TESTSUIT_SERVERPARAMS_H
#define TESTSUIT_SERVERPARAMS_H

#include <iostream>
#include <set>
#include <string>
#include <limits>
#include <filesystem>
#include <fstream>
#include "json/json.hpp"
#include "../implementations/helpers.h"
#include "../implementations/constants.h"

using std::set;
using std::ostream;
using std::string;
using std::ifstream;
using json = nlohmann::json;

struct ServerParams {
    uint32_t pingMs = 0;
    uint32_t maxDownloadSpeed = UINT32_MAX;
    uint32_t maxUploadSpeed = UINT32_MAX;
    uint32_t parallelConnections = 10;
    u_short port = 1234;
    string tempDir = std::filesystem::temp_directory_path().string();
    bool loggingAllowed = true;
    string urlMapFile = "/home/ajk/Desktop/urlMap.json";
    map<string, vector<string>> urlMap;

    map<string, string> additionalKwargs;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
            ServerParams, pingMs, maxDownloadSpeed,
            maxUploadSpeed, parallelConnections, port,
            tempDir, loggingAllowed, urlMapFile,
            additionalKwargs
    )

    void initializeUrlMap(string urlMapFilePath) {
        ifstream is(urlMapFilePath, std::ifstream::ate);
        const auto fileSize = is.tellg();
        if (fileSize > 1 * MB) {
            std::cerr << "url map file exceeds 1MB, abort" << std::endl;
            exit(1);
        }
        is.seekg(0, ifstream::beg);
        json data;
        is >> data;
        is.close();
        urlMap = data.get<map<string, vector<string>>>();
    }

    void initializeFrom(int argc, char *argv[]) {
        for (int i = 1; i < argc; ++i) {
            if (strstr(argv[i], "--pingMs=") != nullptr)
                pingMs = atoi(index(argv[i], '=') + 1);
            else if (strstr(argv[i], "--maxDownloadSpeed=") != nullptr)
                maxDownloadSpeed = atoi(index(argv[i], '=') + 1);
            else if (strstr(argv[i], "--maxUploadSpeed=") != nullptr)
                maxUploadSpeed = atoi(index(argv[i], '=') + 1);
            else if (strstr(argv[i], "--port=") != nullptr)
                port = atoi(index(argv[i], '=') + 1);
            else if (strstr(argv[i], "--parallelConnections=") != nullptr)
                parallelConnections = atoi(index(argv[i], '=') + 1);
            else if (strstr(argv[i], "--loggingAllowed=") != nullptr)
                loggingAllowed = atoi(index(argv[i], '=') + 1);
            else if (strstr(argv[i], "--urlMapFile=") != nullptr)
                urlMapFile = index(argv[i], '=') + 1;
            else if (strstr(argv[i], "--tempDir=") != nullptr)
                tempDir = index(argv[i], '=') + 1;
            else if (strstr(argv[i], "=") != nullptr)
                additionalKwargs[string(argv[i], index(argv[i], '='))] = index(argv[i], '=') + 1;
            else additionalKwargs[argv[i]];
        }
        initializeUrlMap(urlMapFile);
        std::cout << json(*this) << std::endl;
    }
};

#endif //TESTSUIT_SERVERPARAMS_H
