//
// Created by ajk on 08/07/20.
//

#include "Generator.h"

void Generator::get(Long start, Long end, char *memory) {
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

inline char *Generator::toString(Long num, char *ptr, const char *offTheEnd) {
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

inline Pair<Long, int> Generator::spacesBehind(Long index) {
    auto digitChangeIndex = lowerDigitChangeIndex(index);
    auto lowIndex = digitChangeIndex.first;
    auto digits = digitChangeIndex.second;
    auto countOfLowerDigitNumbers = max(0L, pow(10L, digits - 1) - 1);
    auto spaces = countOfLowerDigitNumbers + ((index - lowIndex) / (digits + 1));
    auto extraChars = (index - lowIndex) % (digits + 1);
    return {spaces, static_cast<int>(extraChars)};
}

inline Pair<Long, int> Generator::lowerDigitChangeIndex(Long index) {
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

inline void Generator::generateString(Long start, char *memory, const char *offTheEnd) {
    memory = toString(start, memory, offTheEnd);
    while (memory < offTheEnd) {
        *(memory++) = ' ';
        memory = toString(++start, memory, offTheEnd);
    }
}
