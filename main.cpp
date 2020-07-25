#include <iostream>
#include <fstream>
#include <unistd.h>
#include "server/Server.h"

void asyncTest() {
    thread t([] {
        int i = 0;
        while (++i < 100) {
            usleep(40000);
            std::cout << i << std::endl;
        }
    });
    t.detach();
}

int main() {
    asyncTest();
    usleep(10000000);
    return 0;
}
