#include "Server.h"
#include "../implementations/file/FileOrString.h"

void startServer() {
    ServerSocket server;
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
    socketPtr->close();
}
