#include "ServerParams.h"

void ServerParams::initializeUrlMap(string urlMapFilePath) {
    try {
        ifstream is(urlMapFilePath, std::ifstream::ate);
        if (!is) throw std::runtime_error("url map file not readable");
        const auto fileSize = is.tellg();
        if (fileSize > 2 * MB)
            throw std::runtime_error("url map file exceeds 2 MB, abort");
        is.seekg(0, ifstream::beg);
        json data;
        is >> data;
        is.close();
        vector<UrlMapObject> newUrlMap;
        decltype(allowedParams) extraCommands;
        for (const auto &item: data.items()) {
            newUrlMap.emplace_back(item.value());
            for (const auto &allowedArg: newUrlMap.back().allowedArgs)
                extraCommands[allowedArg.arg] = allowedArg.description;
        }
        urlMap = std::move(newUrlMap);
        urlMapFile = std::move(urlMapFilePath);
        allowedParams.insert(extraCommands.cbegin(), extraCommands.cend());
        print("Url map file updated");
    } catch (...) {
        std::cerr << "Unknown error while setting url map, no changes made" << std::endl;
    }
}

using std::stol;
void ServerParams::initializeFrom(int argc, char **argv) {
    setMaxDownloadSpeed(UINT32_MAX);
    setMaxUploadSpeed(UINT32_MAX);
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