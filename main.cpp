#include <iostream>
#include <fstream>
#include <unistd.h>
#include "server/Server.h"

void runServer() {
    Server server;
    server.start();
    char i = 0;
    std::cin >> i;
    server.stop();
}

int main() {
    runServer();
    return 0;
}
