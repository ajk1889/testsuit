#include <chrono>
#include <fstream>
#include "Generator.h"
#include <iostream>

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

void write() {
    constexpr auto bfr = 1024UL * 1024UL;
    std::ofstream op("nums.bin", std::ios::out | std::ios::binary);
    if (!op) throw std::runtime_error("Failed to open file");
    Generator generator(0);
    auto *memory = new char[bfr];
    for (int i = 0; i < 100; ++i) {
        generator.read(memory, bfr);
        op.write(memory, bfr);
    }
    delete[] memory;
    op.close();
}

void Generator::get(ULong start, ULong end, char *memory) {
    if (start < end) {
        auto offTheEnd = memory + (end - start);
        if (start == 0) {
            generateString(0, memory, offTheEnd);
        } else {
            auto spaces = spacesBehind(start);
            auto startSpaces = spaces.first + 1;
            auto startExtraChars = spaces.second;
            if (startExtraChars == 0) {
                *(memory++) = ' ';
                memory = toString(startSpaces, memory, offTheEnd);
            } else {
                auto digitsToWrite = lenDigits(startSpaces) - startExtraChars + 1;
                auto extractedDigits = startSpaces % pow(10, digitsToWrite);
                for (int len = lenDigits(extractedDigits); len < digitsToWrite; ++len)
                    *(memory++) = '0';
                memory = toString(extractedDigits, memory, offTheEnd);
            }
            if (memory < offTheEnd)
                *(memory++) = ' ';
            generateString(startSpaces + 1, memory, offTheEnd);
        }
    }
}

inline char *Generator::toString(ULong num, char *ptr, const char *offTheEnd) {
    if (ptr < offTheEnd) {
        if (num > 0) {
            auto writePtr = ptr + lenDigits(num);
            while (writePtr > offTheEnd) {
                num /= 10;
                --writePtr;
            }
            auto endPtr = writePtr;
            while (num) {
                *(--writePtr) = static_cast<char>('0' + num % 10);
                num /= 10;
            }
            return endPtr;
        } else {
            *ptr = '0';
            return ptr + 1;
        }
    } else return ptr;
}

inline Pair<ULong, int> Generator::spacesBehind(ULong index) {
    auto digitChangeIndex = lowerDigitChangeIndex(index);
    auto lowIndex = digitChangeIndex.first;
    auto digits = digitChangeIndex.second;
    auto countOfLowerDigitNumbers = max(0L, pow(10L, digits - 1) - 1);
    auto spaces = countOfLowerDigitNumbers + ((index - lowIndex) / (digits + 1));
    auto extraChars = (index - lowIndex) % (digits + 1);
    return {spaces, static_cast<int>(extraChars)};
}

inline Pair<ULong, int> Generator::lowerDigitChangeIndex(ULong index) {
    auto upperIndex = 1UL;
    auto lowerIndex = 1UL;
    auto digits = 0;
    while (index < lowerIndex || index > upperIndex) {
        lowerIndex = upperIndex;
        upperIndex += (digits + 2) * (pow(10L, digits + 1) - pow(10L, digits));
        digits += 1;
    }
    return {lowerIndex, digits};
}

inline void Generator::generateString(ULong start, char *memory, const char *offTheEnd) {
    memory = toString(start, memory, offTheEnd);
    while (memory < offTheEnd) {
        *(memory++) = ' ';
        memory = toString(++start, memory, offTheEnd);
    }
}
