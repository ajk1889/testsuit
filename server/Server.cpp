#include "Server.h"
#include "../implementations/networking/http/response/StringResponse.h"
#include "../implementations/networking/http/response/DescriptorResponse.h"
#include <regex>
#include "boost/algorithm/string.hpp"
#include "../implementations/networking/http/response/FileResponse.h"

using json = nlohmann::json;

shared_ptr<HttpResponse> parseProcessResponse(StreamDescriptor &descriptor) {
    auto rawMetaData = readUntilMatch(descriptor, "\n\n", 8 * KB);
    boost::trim(rawMetaData);
    print("output: ", rawMetaData);
    try {
        auto metaDataJson = json::parse(rawMetaData);
        if (metaDataJson["data"] == "inline") {
            return make_shared<DescriptorResponse>(
                    metaDataJson.value("responseCode", 200),
                    descriptor,
                    metaDataJson.value("headers", map<string, vector<string>>()),
                    metaDataJson.value("length", 0ULL));
        } else {
            return make_shared<FileResponse>(
                    metaDataJson.value("responseCode", 200),
                    metaDataJson["data"],
                    metaDataJson.value("headers", map<string, vector<string>>()),
                    metaDataJson.value("offset", 0ULL),
                    metaDataJson.value("limit", 0ULL));
        }
    } catch (json::parse_error &e) {
        printErr("Error while parsing:", rawMetaData, '\n', "what: ", e.what());
        descriptor.unread(rawMetaData.c_str(), rawMetaData.length());
        return make_shared<DescriptorResponse>(200, descriptor);
    } catch (json::type_error &e) {
        printErr("Error while parsing:", rawMetaData, '\n', "what:", e.what());
        descriptor.unread(rawMetaData.c_str(), rawMetaData.length());
        return make_shared<DescriptorResponse>(200, descriptor);
    }
}

const vector<string> &getCommandForPath(const string &path) {
    auto &urlMap = params.urlMap;
    for (auto &pair: urlMap)
        if (std::regex_match(path, pair.getPathRegex()))
            return pair.command;
    return urlMap[0].command;
}

void Server::handleClient(const SocketPtr &socketPtr) {
    thread([=] {
        try {
            if (params.pingMs != 0)
                std::this_thread::sleep_for(std::chrono::milliseconds(params.pingMs));
            auto request = HttpRequest::from(socketPtr);
            Process process = getCommandForPath(request.path);
            auto input = json(request);
            input["applicationParams"] = params;
            print("input: ", input);
            auto output = process.run((input.dump() + "\n").c_str());
            parseProcessResponse(*output)->writeTo(*socketPtr);
        } catch (std::runtime_error &e) {
            printErr("Runtime Error", e.what());
            try {
                std::ostringstream data("<H1>Internal server error</H1>");
                data << "<H3>Description</H3>";
                data << e.what();
                StringResponse response(ResponseCode::InternalServerError, data.str());
                response.writeTo(*socketPtr);
            } catch (...) {
                printErr("Exception while handling previous exception");
            }
        } catch (...) {
            try {
                StringResponse response(ResponseCode::InternalServerError,
                                        "<H1>Internal server error</H1><H3>Unknown exception</H3>");
                response.writeTo(*socketPtr);
            } catch (...) {
                printErr("Unknown error");
            }
        }
        socketPtr->close();
    }).detach();
}

void Server::startAsync() {
    serverSocket = make_shared<ServerSocket>(this, params.port, params.parallelConnections);
    clientAcceptor = thread([=] {
        while (isRunning) {
            auto client = serverSocket->accept({0, 1000});
            if (client) handleClient(client);
        }
    });
    clientAcceptor.detach();
}


void Server::startSync() {
    serverSocket = make_shared<ServerSocket>(this, params.port, params.parallelConnections);
    while (isRunning) {
        auto client = serverSocket->accept({0, 1000});
        if (client) handleClient(client);
    }
}

void printUnknownCommandError(const string &command, const map<string, string> &allowedParams) {
    std::cout << "Invalid command " << command << '\n'
              << "\nALLOWED COMMANDS" << '\n'
              << "stop:  " << "Stop server" << '\n'
              << "remap:  " << "Reloads the URL Map file" << '\n'
              << "\nALLOWED PARAMETERS " << "(Usage: `parameter=value`)" << '\n';
    for (const auto &param : allowedParams)
        std::cout << param.first << ": " << param.second << '\n';
    std::cout.flush();
}

void Server::execute(const string &command) {
    auto separatorPos = command.find('=');
    if (command == "remap") {
        params.initializeUrlMap(params.urlMapFile);
    } else if (separatorPos != string::npos) {
        try {
            auto parameter = command.substr(0, separatorPos);
            auto value = command.substr(separatorPos + 1);
            if (parameter == "maxdspeed") {
                params.setMaxDownloadSpeed(std::stol(value));
                std::cout << "Max download speed set to " << params.getMaxDownloadSpeed() << "KB/s" << std::endl;
            } else if (parameter == "random-dspeed") {
                params.setVaryingUploadSpeed(value);
            }  else if (parameter == "maxuspeed") {
                params.setMaxUploadSpeed(std::stol(value));
                std::cout << "Max upload speed set to " << params.getMaxUploadSpeed() << "KB/s" <<  std::endl;
            } else if (parameter == "logging") {
                params.loggingAllowed = std::stoi(value);
                std::cout << "Logging Allowed: " << params.loggingAllowed << std::endl;
            } else if (parameter == "ping") {
                params.pingMs = std::stol(value);
                std::cout << "Ping changed to: " << params.pingMs << " milliseconds" << std::endl;
            } else if (parameter == "temp-dir") {
                params.tempDir = value;
                std::cout << "Temp dir changed to: " << params.tempDir << std::endl;
            } else if (parameter == "url-map") {
                params.initializeUrlMap(value);
                std::cout << "URL map re-initialized from " << params.urlMapFile << std::endl;
            } else {
                if (params.allowedParams.find(parameter) != params.allowedParams.cend()) {
                    params.additionalKwargs[parameter] = value;
                    std::cout << parameter << "is set to " << value << std::endl;
                } else printUnknownCommandError(command, params.allowedParams);
            }
        } catch (...) {
            std::cout << "Unknown error while setting argument " << command << std::endl;
        }
    } else printUnknownCommandError(command, params.allowedParams);
}
