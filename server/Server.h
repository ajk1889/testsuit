#ifndef TESTSUIT_SERVER_H
#define TESTSUIT_SERVER_H

#include "../implementations/networking/ServerSocket.h"
#include "../implementations/networking/Socket.h"
#include "../server/ServerParams.h"
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

    void handleClient(SocketPtr socketPtr) {

    }

    void handleClientAsync(SocketPtr socketPtr) {
        thread handler([&] { handleClient(socketPtr); });
        handler.detach();
    }

    shared_ptr<ServerSocket> serverSocket;
public:
    ServerParams params;
    bool isRunning = true;

    static void test();

    void start() {
        serverSocket = make_shared<ServerSocket>(params.port, params.maxDownloadSpeed);
        thread clientAcceptor([&] {
            while (isRunning)
                handleClientAsync(serverSocket->accept());
        });
        clientAcceptor.detach();
    }
};


#endif //TESTSUIT_SERVER_H
