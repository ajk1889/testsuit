#include <iostream>
#include "server/Server.h"

void runServer() {
    Server server;
    server.start();
    char i = 0;
    std::cin >> i;
    server.stop();
}
int main() {
//    print(find("defghijklmn", "def", 11, "abc"));
    runServer();
    return 0;
}
