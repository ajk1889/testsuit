#ifndef TESTSUIT_SERVER_H
#define TESTSUIT_SERVER_H

#include "../implementations/networking/ServerSocket.h"
#include "../implementations/networking/Socket.h"
#include "../server/ServerParams.h"
#include "../implementations/networking/http/request/HttpRequest.h"
#include <thread>
#include <set>
#include <iostream>
#include <memory>
#include <atomic>

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
    std::atomic<bool> isRunning = true;

    Server() = default;

    static void test();

    void startAsync();

    void startSync();

    void stop() {
        isRunning = false;
        using std::chrono_literals::operator ""ms;
        std::this_thread::sleep_for(2ms);
        // waiting for socket to close
        curl_easy_cleanup(getCurl());
    }

    static void execute(const string &command);
};


#endif //TESTSUIT_SERVER_H
