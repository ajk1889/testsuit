#include <iostream>
#include "server/Server.h"

void runServer() {
    Server server;
    server.start();
    string command;
    while (true) {
        std::cin >> command;
        if (command == "stop") {
            server.stop();
            break;
        } else Server::execute(command);
    }
}

int main(int argc, char *argv[]) {
    params.initializeFrom(argc, argv);
    runServer();
    return 0;
}
