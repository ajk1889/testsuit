#include <iostream>
#include "Generator.h"
#include <chrono>
int main() {
    constexpr auto bfr = 100*1024;
    constexpr auto size = 2UL * 1024UL * 1024UL * 1024UL;

    auto *memory = new unsigned char[100 * 1024];
    auto read = 0UL;

    auto t_start = std::chrono::high_resolution_clock::now();
    while (read<size){
        Generator::get(read, read+bfr, memory);
        read += bfr;
    }
    auto t_end = std::chrono::high_resolution_clock::now();

    std::cout<<std::chrono::duration<double, std::milli>(t_end-t_start).count()<<std::endl;
    return 0;
}
