#ifndef TESTSUIT_SERVER_H
#define TESTSUIT_SERVER_H

#include "../implementations/networking/ServerSocket.h"
#include "../implementations/networking/Socket.h"
#include <thread>
#include <set>
#include <iostream>

using std::thread;

class Server {
    friend void startServer();

    friend void connectToServer();

public:
    static void test();

    void start() {

    }
};


#endif //TESTSUIT_SERVER_H
