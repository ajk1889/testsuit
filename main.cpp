#include <iostream>
#include <fstream>
#include <unistd.h>
#include "server/Server.h"

int main() {
    Server server;
    server.start();
    char i = 0;
    std::cin >> i;
    server.stop();
    return 0;
}
