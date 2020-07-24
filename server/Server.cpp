//
// Created by ajk on 08/07/20.
//

#include "Server.h"

void Server::startServer() {
    ServerSocket server;
    Socket client = server.accept();
    int number;
    std::cout << client.read(number) << std::endl;
    client.write(-81273);
}

void Server::connect() {
    Socket socket("127.0.0.1", 1234);
    socket.write(10000);
    int number;
    std::cout << socket.read(number) << std::endl;
}

void Server::test() {
    thread server(startServer);
    thread client(connect);
    server.join();
    client.join();
}
