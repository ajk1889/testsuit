#include "ServerParams.h"

void ServerParams::initializeUrlMap(string urlMapFilePath) {
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
            extraArgsCmd.emplace_back("--list-params");
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

using std::stol;
void ServerParams::initializeFrom(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        if (strstr(argv[i], "--pingMs=") != nullptr)
            pingMs = stol(index(argv[i], '=') + 1);
        else if (strstr(argv[i], "--maxdspeed=") != nullptr)
            setMaxDownloadSpeed(stol(index(argv[i], '=') + 1));
        else if (strstr(argv[i], "--maxuspeed=") != nullptr)
            setMaxUploadSpeed(stol(index(argv[i], '=') + 1));
        else if (strstr(argv[i], "--port=") != nullptr)
            port = stol(index(argv[i], '=') + 1);
        else if (strstr(argv[i], "--parallelConnections=") != nullptr)
            parallelConnections = stol(index(argv[i], '=') + 1);
        else if (strstr(argv[i], "--logging=") != nullptr)
            loggingAllowed = stol(index(argv[i], '=') + 1);
        else if (strstr(argv[i], "--urlMapFile=") != nullptr)
            urlMapFile = index(argv[i], '=') + 1;
        else if (strstr(argv[i], "--tempDir=") != nullptr)
            tempDir = index(argv[i], '=') + 1;
        else if (strstr(argv[i], "=") != nullptr)
            additionalKwargs[string(argv[i], index(argv[i], '='))] = index(argv[i], '=') + 1;
        else additionalKwargs[argv[i]];
    }
    if (tempDir.empty())
        tempDir = std::filesystem::temp_directory_path().string() + "/testsuit";
    initializeUrlMap(urlMapFile);
    std::cout << json(*this) << std::endl;
}

ServerParams params;