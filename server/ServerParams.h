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

struct ServerParams {
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

    void initializeUrlMap(string urlMapFilePath) {
        try {
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
            auto newUrlMap = data.get<map<string, vector<string>>>();
            decltype(allowedParams) extraCommands;
            for (const auto &pair: newUrlMap) {
                auto extraArgsCmd = pair.second;
                extraArgsCmd.push_back("--list-params");
                auto response = readUntilMatch(*Process(extraArgsCmd).run("\n"), "\n\n");
                boost::trim(response);
                if (response.empty()) continue;
                json commands = json::parse(response);
                for (auto it = commands.cbegin(); it != commands.cend(); it++)
                    extraCommands[it.key()] = it.value();
            }
            urlMap = std::move(newUrlMap);
            urlMapFile = std::move(urlMapFilePath);
            allowedParams.insert(extraCommands.cbegin(), extraCommands.cend());
        } catch (...) {
            print("Unknown error while setting url map, no changes made");
        }
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
                initializeUrlMap(index(argv[i], '=') + 1);
            else if (strstr(argv[i], "--tempDir=") != nullptr)
                tempDir = index(argv[i], '=') + 1;
            else if (strstr(argv[i], "=") != nullptr)
                additionalKwargs[string(argv[i], index(argv[i], '='))] = index(argv[i], '=') + 1;
            else additionalKwargs[argv[i]];
        }
        if (tempDir.empty())
            tempDir = std::filesystem::temp_directory_path().string() + "/testsuit";
        std::cout << json(*this) << std::endl;
    }
};

#endif //TESTSUIT_SERVERPARAMS_H
