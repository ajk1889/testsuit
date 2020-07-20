#include <iostream>
#include "implementations/Generator.h"
#include <chrono>
#include <fstream>

void visualize() {
    char memory[51];
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

void write() {
    constexpr auto bfr = 1024UL * 1024UL;
    std::ofstream op("nums.bin", std::ios::out | std::ios::binary);
    if (!op) {
        std::cout << "Failed to open file";
        return;
    }
    Generator generator(0);
    auto *memory = new char[bfr];
    for (int i = 0; i < 100; ++i) {
        generator.read(memory, bfr);
        op.write(memory, bfr);
    }
    delete[] memory;
    op.close();
}

int main() {
//    visualize();
    performanceTest();
//    write();
    return 0;
}
