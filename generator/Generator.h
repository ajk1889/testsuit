#ifndef TESTSUIT_GENERATOR_H
#define TESTSUIT_GENERATOR_H

#include "Pair.h"
#include <memory>
#include <cmath>
#include <iostream>

using ULong = unsigned long long;

class Generator {
    friend void visualize();

    friend void performanceTest();

private:
    static Pair<ULong, int> lowerDigitChangeIndex(ULong index);

    static Pair<ULong, int> spacesBehind(ULong index);

    static char *toString(ULong num, char *ptr, const char *offTheEnd);

    static void generateString(ULong start, char *memory, const char *offTheEnd);

    static void get(ULong start, ULong end, char *memory);

    inline static uint lenDigits(ULong num) {
        return static_cast<uint>(std::floor(std::log10(num))) + 1;
    }

    inline static ULong pow(ULong base, uint exp) {
        ULong ans = 1;
        exp += 1;
        while (--exp) ans *= base;
        return ans;
    }

    inline static ULong max(ULong a, ULong b) {
        return a > b ? a : b;
    }

    ULong seek;

public:
    explicit Generator(ULong startIndex = 0) : seek(startIndex) {}

    void read(char *buffer, uint bytes) {
        get(seek, seek + bytes, buffer);
        seek += bytes;
    }
};

#endif //TESTSUIT_GENERATOR_H
