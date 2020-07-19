#include <iostream>
#include "Generator.h"
#include <chrono>

void visualize() {
    unsigned char memory[101];
    memory[100] = '\0';
    for (int i = 0; i < 5; ++i) {
        Generator::get(i * 100, (i + 1) * 100, memory);
        std::cout << memory << std::endl;
    }
    memory[5] = '\0';
    Generator::get(3872, 3872, memory);
    std::cout << memory << std::endl;
}

void performanceTest() {
    constexpr auto bfr = 200UL * 1024UL;
    constexpr auto size = 500UL * 1024UL * 1024UL;

    auto *memory = new unsigned char[bfr];
    auto read = 0UL;

    auto t_start = std::chrono::high_resolution_clock::now();
    while (read < size) {
        Generator::get(read, read + bfr, memory);
        read += bfr;
    }
    auto t_end = std::chrono::high_resolution_clock::now();
    delete[] memory;

    std::cout << std::chrono::duration<double, std::milli>(t_end - t_start).count() << std::endl;
}

int main() {
    visualize();
    performanceTest();
    return 0;
}
