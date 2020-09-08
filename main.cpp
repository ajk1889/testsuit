#include <iostream>
#include "server/Server.h"
#include "test.h"

void runServer() {
    Server server;
    server.start();
    char i = 0;
    std::cin >> i;
    server.stop();
}
int main() {
    char c1[] = "python3";
    char c2[] = "-c";
    char c3[] = "print(input())";
    char *arr[] = {c1, c2, c3, nullptr};
    createChild("python3", arr, "Hello world\n");
    return 0;
}
