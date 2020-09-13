#include <iostream>
#include "server/Server.h"

void runServer(int argc, char *argv[]) {
    Server server(argc, argv);
    server.start();
    string command;
    while (true) {
        std::cin >> command;
        if (command == "stop") {
            server.stop();
            break;
        } else server.execute(command);
    }
}

int main(int argc, char *argv[]) {
    runServer(argc, argv);
    return 0;
}
