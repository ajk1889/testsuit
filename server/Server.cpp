#include "Server.h"
#include "../implementations/networking/http_response/HttpResponse.h"
#include "../implementations/Process.h"
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

void Server::handleClient(const SocketPtr &socketPtr) {
    try {
        auto request = HttpRequest::from(socketPtr);
        Process process = socketPtr->server->params.urlMap.find(request.path)->second;
        string input = json(request).dump();
        print(input);
        char data[1024];
        data[process.run(input.c_str())->read(data, 1023)] = '\0';
        HttpResponse response(ResponseCode::OK, data);
        *socketPtr << response;
    } catch (std::runtime_error &e) {
        std::ostringstream data("<H1>Internal server error</H1>");
        data << "<H3>Description</H3>";
        data << e.what();
        HttpResponse response(ResponseCode::InternalServerError, data.str());
        *socketPtr << response;
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
