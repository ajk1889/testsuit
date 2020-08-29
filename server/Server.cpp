#include "Server.h"
#include "../implementations/networking/HttpResponse.h"
#include <filesystem>

void startServer() {
    ServerSocket server;
    std::filesystem::path uploadsDir(server.server->params.uploadsPath);
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
    auto request = HttpRequest::from(socketPtr);
    std::cout << "Path: " << request.path << std::endl;
    for (auto &item: request.GET)
        std::cout << "GET[" << item.first << "] = " << item.second << std::endl;
    for (auto &item: request.POST)
        std::cout << "POST[" << item.first << "] = " << item.second.data << std::endl;
    HttpResponse response("<H1>Success</H1>");
    *socketPtr << response;
    socketPtr->close();
}
