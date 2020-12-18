#include <iostream>
#include "server/Server.h"

void runServer() {
    Server server;
    if (!params.disableStdin) {
        server.startAsync();
        string command;
        while (true) {
            std::getline(std::cin, command, '\n');
            if (command == "stop") {
                server.stop();
                break;
            } else Server::execute(command);
        }
    } else server.startSync();
}

int main(int argc, char *argv[]) {
    params.initializeFrom(argc, argv);
    runServer();
    return 0;
}
