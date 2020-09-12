#include "Server.h"
#include "../implementations/Process.h"
#include "../implementations/networking/http_response/StringResponse.h"
#include "../implementations/networking/http_response/DescriptorResponse.h"
#include <filesystem>

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
    char data[1024];
    data[descriptor.read(data, 1023)] = '\0';
    return make_shared<StringResponse>(200, data);
//    auto metaData = readUntilMatch(descriptor, "\n\n", 8*KB);
//    if(metaData.rfind("\n\n") == string::npos) {
//        return DescriptorResponse(200, descriptor);
//    }
}

void Server::handleClient(const SocketPtr &socketPtr) {
    try {
        auto request = HttpRequest::from(socketPtr);
        auto urlMap = socketPtr->server->params.urlMap;
        auto command = urlMap.find(request.path);
        if (command == urlMap.cend())
            command = urlMap.find("default");
        Process process = command->second;
        auto input = json(request).dump() + "\n";
        parseProcessResponse(*process.run(input.c_str()))->writeTo(*socketPtr);
    } catch (std::runtime_error &e) {
        std::ostringstream data("<H1>Internal server error</H1>");
        data << "<H3>Description</H3>";
        data << e.what();
        StringResponse response(ResponseCode::InternalServerError, data.str());
        response.writeTo(*socketPtr);
    } catch (...) {
        std::ostringstream data("<H1>Internal server error</H1>");
        data << "<H3>Unknown exception</H3>";
        StringResponse response(ResponseCode::InternalServerError, data.str());
        response.writeTo(*socketPtr);
    }
    socketPtr->close();
}

void Server::start() {
    serverSocket = make_shared<ServerSocket>(this, params.port, params.parallelConnections);
    clientAcceptor = thread([&] {
        while (isRunning)
            handleClient(serverSocket->accept());
    });
    clientAcceptor.detach();
}
