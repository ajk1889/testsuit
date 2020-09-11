#ifndef TESTSUIT_SERVER_H
#define TESTSUIT_SERVER_H

#include "../implementations/networking/ServerSocket.h"
#include "../implementations/networking/Socket.h"
#include "../server/ServerParams.h"
#include "../implementations/networking/HttpRequest.h"
#include <thread>
#include <set>
#include <iostream>
#include <memory>

using std::thread;
using std::shared_ptr;
using std::make_shared;
using SocketPtr = std::shared_ptr<Socket>;

class Server {
    friend void startServer();

    friend void connectToServer();

    static void handleClient(const SocketPtr &socketPtr);

    shared_ptr<ServerSocket> serverSocket;
    thread clientAcceptor;
public:
    ServerParams params;
    bool isRunning = true;

    Server() = default;

    Server(int argc, char *argv[]) {
        params.initializeFrom(argc, argv);
    }

    static void test();

    void start();

    void stop() {
        isRunning = false;
        serverSocket->close();
    }
};


#endif //TESTSUIT_SERVER_H
