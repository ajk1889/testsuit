#include "implementations/utils/Process.h"
#include "server/Server.h"

void Process::test() {
    Process process = {"python3", "-c", "print(int(input())**237)"};
    char data[1024]{};
    data[0] = '\0';
    auto descriptor = process.run("1820\n");
    if (descriptor) data[descriptor->read(data, 1023)] = '\0';
    else std::cout << "Null descriptor" << std::endl;
    std::cout << data << std::endl;
}


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