#include <iostream>
#include "lib/json.hpp"
//#include "server/Server.h"
//#include "implementations/Process.h"
//
//void runServer() {
//    Server server;
//    server.start();
//    char i = 0;
//    std::cin >> i;
//    server.stop();
//}
using json = nlohmann::json;
int main() {
    auto data = json::parse(R"({"name":"json","items":[1,2,3],"another":{"a":"b"}})");
    data["abc"] = "asd";
    std::cout << data << std::endl;
    return 0;
}
