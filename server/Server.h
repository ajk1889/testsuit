//
// Created by ajk on 08/07/20.
//

#ifndef TESTSUIT_SERVER_H
#define TESTSUIT_SERVER_H

#include "../implementations/networking/ServerSocket.h"
#include "../implementations/networking/Socket.h"
#include <thread>
#include <set>
#include <iostream>

using std::thread;

class Server {
    static void startServer();

    static void connect();

public:
    static void test();

    void start() {

    }
};


#endif //TESTSUIT_SERVER_H
