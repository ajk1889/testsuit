#include "Server.h"
#include "../implementations/Process.h"
#include "../implementations/networking/http_response/StringResponse.h"
#include "../implementations/networking/http_response/DescriptorResponse.h"
#include <filesystem>
#include "boost/algorithm/string.hpp"
#include "../implementations/networking/http_response/FileResponse.h"

using json = nlohmann::json;

void startServer() {
    ServerSocket server;
    std::filesystem::path uploadsDir(server.server->params.tempDir);
    if (!std::filesystem::exists(uploadsDir) && !std::filesystem::create_directory(uploadsDir))
        throw std::runtime_error("Unable to create uploads directory");
    auto client = server.accept();
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
    } catch (json::parse_error e) {
        std::cerr << "Error while parsing: " << rawMetaData << "\n"
                  << "what: " << e.what() << std::endl;
        return make_shared<DescriptorResponse>(200, descriptor);
    } catch (json::type_error e) {
        std::cerr << "Error while parsing: " << rawMetaData << "\n"
                  << "what: " << e.what() << std::endl;
        return make_shared<DescriptorResponse>(200, descriptor);
    }
}

void Server::handleClient(const SocketPtr &socketPtr) {
    thread([=] {
        try {
            auto request = HttpRequest::from(socketPtr);
            print("Request:", request.requestType, request.path,
                  "GET:", json(request.GET), "POST:", json(request.POST));
            auto urlMap = socketPtr->server->params.urlMap;
            auto command = urlMap.find(request.path);
            if (command == urlMap.cend())
                command = urlMap.find("default");
            Process process = command->second;
            auto input = json(request).dump() + "\n";
            auto output = process.run(input.c_str());
            parseProcessResponse(*output)->writeTo(*socketPtr);
        } catch (std::runtime_error &e) {
            std::ostringstream data("<H1>Internal server error</H1>");
            data << "<H3>Description</H3>";
            data << e.what();
            StringResponse response(ResponseCode::InternalServerError, data.str());
            response.writeTo(*socketPtr);
        } catch (...) {
            StringResponse response(ResponseCode::InternalServerError,
                                    "<H1>Internal server error</H1><H3>Unknown exception</H3>");
            response.writeTo(*socketPtr);
        }
        socketPtr->close();
    }).detach();
}

void Server::start() {
    serverSocket = make_shared<ServerSocket>(this, params.port, params.parallelConnections);
    clientAcceptor = thread([&] {
        while (isRunning) handleClient(serverSocket->accept());
    });
    clientAcceptor.detach();
}
