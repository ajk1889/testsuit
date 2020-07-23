//
// Created by ajk on 08/07/20.
//

#ifndef TESTSUIT_SERVER_H
#define TESTSUIT_SERVER_H

#include "implementations/ServerSocket.h"
#include "implementations/Socket.h"
#include <thread>
#include <iostream>

using std::thread;

struct Server {
    static void startServer() {
        ServerSocket server;
        Socket client = server.accept();
        char bfr[256];
        bfr[client.read(bfr, 255)] = '\0';
        std::cout << bfr << std::endl;
        client.write("hello world! from server");
    }

    static void connect() {
        Socket socket("127.0.0.1", 1234);
        socket.write("hello world! from client");
        char bfr[256];
        bfr[socket.read(bfr, 255)] = '\0';
        std::cout << bfr << std::endl;
    }

    static void test() {
        thread server(startServer);
        thread client(connect);
        server.join();
        client.join();
    }
};


#endif //TESTSUIT_SERVER_H
