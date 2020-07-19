//
// Created by ajk on 08/07/20.
//

#ifndef TESTSUIT_GENERATOR_H
#define TESTSUIT_GENERATOR_H
#include "Pair.h"
#include <memory>
#include <cmath>
#include <iostream>

typedef unsigned long long Long;

class Generator {
    friend void visualize();

    friend void performanceTest();

private:
    static Pair<Long, int> lowerDigitChangeIndex(Long index);

    static Pair<Long, int> spacesBehind(Long index);

    static uint toString(Long num, char *ptr, uint freeMemory);

    static void generateString(Long start, Long end, char *memory, int freeMemory);

    static void get(Long start, Long end, char *memory);

    inline static uint lenDigits(Long num) {
        return static_cast<uint>(std::floor(std::log10(num))) + 1;
    }

    inline static Long pow(Long base, uint exp) {
        Long ans = 1;
        exp += 1;
        while (--exp) ans *= base;
        return ans;
    }

    inline static Long max(Long a, Long b) {
        return a > b ? a : b;
    }

    Long seek = 0L;

public:
    explicit Generator(Long startIndex = 0) : seek(startIndex) {}

    void read(char *buffer, uint bytes) {
        get(seek, seek + bytes, buffer);
        seek += bytes;
    }
};


#endif //TESTSUIT_GENERATOR_H
