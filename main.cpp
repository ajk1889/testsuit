#include <iostream>
#include <fstream>
#include <unistd.h>
#include "server/Server.h"

thread *asyncTest() {
    return new thread([] {
        for (int i = 0; i < 120; ++i) {
            usleep(30000);
            std::cout << i << std::endl;
        }
    });
}

int main() {
    auto thread = asyncTest();
    usleep(3000000);
    delete thread;
    return 0;
}
