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
    Process p{"python3", "-c", "print(input())"};
    char data[1024];
    data[p.run("Hello world\n")->read(data, 1023)] = '\0';
    std::cout << data << std::endl;
    return 0;
}
