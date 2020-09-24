#include "Server.h"
#include "../implementations/utils/Process.h"
#include "../implementations/networking/http/response/StringResponse.h"
#include "../implementations/networking/http/response/DescriptorResponse.h"
#include <filesystem>
#include "boost/algorithm/string.hpp"
#include "../implementations/networking/http/response/FileResponse.h"

using json = nlohmann::json;

void startServer() {
    ServerSocket server;
    shared_ptr<Socket> client;
    while (!client) client = server.accept({0, 1000});
    int number;
    std::cout << client->read(number) << std::endl;
    client->write(-81273);
}

void connectToServer() {
    Socket socket("127.0.0.1", 1234);
    socket.write(10000);
    int number;
    std::cout << socket.read(number) << std::endl;
}

void Server::test() {
    thread server(startServer);
    thread client(connectToServer);
    server.join();
    client.join();
}

shared_ptr<HttpResponse> parseProcessResponse(StreamDescriptor &descriptor) {
    auto rawMetaData = readUntilMatch(descriptor, "\n\n", 8 * KB);
    boost::trim(rawMetaData);
    print("output: ", rawMetaData);
    map<string, vector<string>> emptyHeader;
    try {
        auto metaDataJson = json::parse(rawMetaData);
        if (metaDataJson["data"] == "inline") {
            return make_shared<DescriptorResponse>(
                    metaDataJson.value("responseCode", 200),
                    descriptor,
                    metaDataJson.value("headers", emptyHeader),
                    metaDataJson.value("length", 0ULL));
        } else {
            return make_shared<FileResponse>(
                    metaDataJson.value("responseCode", 200),
                    metaDataJson["data"],
                    metaDataJson.value("headers", emptyHeader),
                    metaDataJson.value("offset", 0ULL),
                    metaDataJson.value("limit", 0ULL));
        }
    } catch (json::parse_error &e) {
        std::cerr << "Error while parsing: " << rawMetaData << "\n"
                  << "what: " << e.what() << std::endl;
        return make_shared<DescriptorResponse>(200, descriptor);
    } catch (json::type_error &e) {
        std::cerr << "Error while parsing: " << rawMetaData << "\n"
                  << "what: " << e.what() << std::endl;
        return make_shared<DescriptorResponse>(200, descriptor);
    }
}

void Server::handleClient(const SocketPtr &socketPtr) {
    thread([=] {
        try {
            auto request = HttpRequest::from(socketPtr);
            auto urlMap = params.urlMap;
            auto command = urlMap.find(request.path);
            if (command == urlMap.cend())
                command = urlMap.find("default");
            Process process = command->second;
            auto input = json(request);
            input["applicationParams"] = params;
            print("input: ", input);
            auto output = process.run((input.dump() + "\n").c_str());
            parseProcessResponse(*output)->writeTo(*socketPtr);
        } catch (std::runtime_error &e) {
            std::cerr << "Unknown error " << e.what() << std::endl;
            try {
                std::ostringstream data("<H1>Internal server error</H1>");
                data << "<H3>Description</H3>";
                data << e.what();
                StringResponse response(ResponseCode::InternalServerError, data.str());
                response.writeTo(*socketPtr);
            } catch (...) {
                std::cerr << "Exception while handling previous exception" << std::endl;
            }
        } catch (...) {
            try {
                StringResponse response(ResponseCode::InternalServerError,
                                        "<H1>Internal server error</H1><H3>Unknown exception</H3>");
                response.writeTo(*socketPtr);
            } catch (...) {
                std::cerr << "Unknown error" << std::endl;
            }
        }
        socketPtr->close();
    }).detach();
}

void Server::start() {
    serverSocket = make_shared<ServerSocket>(this, params.port, params.parallelConnections);
    std::filesystem::path uploadsDir(params.tempDir);
    if (!std::filesystem::exists(uploadsDir) && !std::filesystem::create_directory(uploadsDir))
        throw std::runtime_error("Unable to create uploads directory");
    clientAcceptor = thread([=] {
        while (isRunning) {
            auto client = serverSocket->accept({0, 1000});
            if (client) handleClient(client);
        }
    });
    clientAcceptor.detach();
}

void printUnknownCommandError(const string &command, const map<string, string> &allowedParams) {
    print("Invalid command", command);
    print("\nALLOWED COMMANDS");
    print("stop: ", "Stop server");
    print("remap: ", "Reloads the URL Map file");
    print("\nALLOWED PARAMETERS", "(Usage: `parameter=value`)");
    for (const auto &param : allowedParams)
        print(param.first, ":", param.second);
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
                print("Max download speed set to", params.getMaxDownloadSpeed());
            } else if (parameter == "maxuspeed") {
                params.setMaxDownloadSpeed(std::stol(value));
                print("Max upload speed set to", params.getMaxDownloadSpeed());
            } else if (parameter == "logging") {
                params.loggingAllowed = std::stoi(value);
                print("Logging Allowed:", params.loggingAllowed);
            } else if (parameter == "ping") {
                params.pingMs = std::stol(value);
                print("Ping changed to:", params.pingMs, "milli-seconds");
            } else if (parameter == "temp-dir") {
                params.tempDir = value;
                print("Temp dir changed to:", params.tempDir);
            } else if (parameter == "urlmap") {
                params.initializeUrlMap(value);
                print("URL map re-initialized from", params.urlMapFile);
            } else {
                if (params.allowedParams.find(parameter) != params.allowedParams.cend())
                    params.additionalKwargs[parameter] = value;
                else printUnknownCommandError(command, params.allowedParams);
            }
        } catch (...) {
            print("Unknown error while setting argument", command);
        }
    } else printUnknownCommandError(command, params.allowedParams);
}
