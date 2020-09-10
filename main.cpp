#include <iostream>
#include "server/Server.h"
#include "implementations/Process.h"

void runServer() {
    Server server;
    server.start();
    char i = 0;
    std::cin >> i;
    server.stop();
}
int main() {
    Process::test();
    return 0;
}
