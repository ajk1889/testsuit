//
// Created by ajk on 08/07/20.
//

#ifndef TESTSUIT_SERVER_H
#define TESTSUIT_SERVER_H

#include "../implementations/networking/ServerSocket.h"
#include "../implementations/networking/Socket.h"
#include <thread>
#include <iostream>

using std::thread;

struct Server {
    static void startServer() {
        ServerSocket server;
        Socket client = server.accept();
        int number;
        std::cout << client.read(number) << std::endl;
        client.write(81273);
    }

    static void connect() {
        Socket socket("127.0.0.1", 1234);
        socket.write(10000);
        int number;
        std::cout << socket.read(number) << std::endl;
    }

    static void test() {
        thread server(startServer);
        thread client(connect);
        server.join();
        client.join();
    }
};


#endif //TESTSUIT_SERVER_H
