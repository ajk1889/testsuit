#include "implementations/utils/Process.h"
#include "server/Server.h"
#include "implementations/Generator.h"

void Process::test() {
    Process process = {"python3", "-c", "print(int(input())**237)"};
    char data[1024]{};
    data[0] = '\0';
    auto descriptor = process.run("1820\n");
    if (descriptor) data[descriptor->read(data, 1023)] = '\0';
    else std::cout << "Null descriptor" << std::endl;
    std::cout << data << std::endl;
}


void startServer() {
    ServerSocket server;
    shared_ptr<Socket> client;
    while (!client) client = server.accept({0, 1000});
    int number;
    std::cout << client->read(number) << std::endl;
    client->write(-81273);
}

void connectToServer() {
    Socket socket("127.0.0.1", 1234);
    socket.write(10000);
    int number;
    std::cout << socket.read(number) << std::endl;
}

void Server::test() {
    thread server(startServer);
    thread client(connectToServer);
    server.join();
    client.join();
}

void visualize() {
    char memory[51]{};
    memory[50] = '\0';
    for (int i = 0; i < 5; ++i) {
        Generator::get(i * 50, (i + 1) * 50, memory);
        std::cout << memory << std::endl;
    }
}

void performanceTest() {
    constexpr auto bfr = 2 * 1024UL * 1024UL;
    constexpr auto size = 2 * 1024UL * 1024UL * 1024UL;

    auto *memory = new char[bfr];
    auto read = 0UL;

    auto t_start = std::chrono::high_resolution_clock::now();
    Generator generator(0);
    while (read < size) {
        generator.read(memory, bfr);
        read += bfr;
    }
    auto t_end = std::chrono::high_resolution_clock::now();
    delete[] memory;

    std::cout << std::chrono::duration<double, std::milli>(t_end - t_start).count() << std::endl;
}
