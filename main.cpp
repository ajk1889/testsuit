#include <iostream>
#include "server/Server.h"

void runServer(int argc, char *argv[]) {
    Server server(argc, argv);
    server.start();
    char i = 0;
    std::cin >> i;
    server.stop();
}

int main(int argc, char *argv[]) {
    runServer(argc, argv);
    return 0;
}
