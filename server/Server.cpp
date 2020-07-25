#include "Server.h"

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
