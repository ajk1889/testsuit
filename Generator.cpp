//
// Created by ajk on 08/07/20.
//

#include "Generator.h"

void Generator::get(Long start, Long end, Char *memory) {
    if (start == 0) {
        auto totalSpaces = spacesBehind(end).first + 2;
        generateString(0, totalSpaces, memory, end);
    } else {
        auto spaces = spacesBehind(start);
        auto startSpaces = spaces.first + 1;
        auto startExtraChars = spaces.second;
        auto endSpaces = spacesBehind(end).first + 1;
        Long freeMemory = end - start;
        Long written;
        if (startExtraChars == 0) {
            *memory = ' ';
            written = 1 + toString(startSpaces % pow(10, lenDigits(startSpaces) - startExtraChars),
                                   memory + 1, static_cast<uint>(freeMemory - 1));
        } else {
            written = toString(startSpaces % pow(10, lenDigits(startSpaces) - startExtraChars + 1),
                               memory, static_cast<uint>(freeMemory));
        }
        if (freeMemory > written)
            memory[written++] = ' ';

        generateString(startSpaces + 1, endSpaces + 1,
                       memory + written, static_cast<uint>(freeMemory - written));
    }
}

inline uint Generator::toString(Long num, Char *ptr, uint freeMemory) {
    if (num > 0) {
        uint charLen = lenDigits(num);
        while (charLen > freeMemory) {
            num /= 10;
            --charLen;
        }
        ptr += charLen;
        while (num) {
            *(--ptr) = '0' + num % 10;
            num /= 10;
        }
        return charLen;
    } else if (freeMemory > 0) {
        *ptr = '0';
        return 1;
    } else return 0;
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
    auto upperIndex = 1L;
    auto lowerIndex = 1L;
    auto digits = 0;
    while (index < lowerIndex || index > upperIndex) {
        lowerIndex = upperIndex;
        upperIndex += (digits + 2) * (pow(10L, digits + 1) - pow(10L, digits));
        digits += 1;
    }
    return {lowerIndex, digits};
}

void Generator::generateString(Long start, Long end, Char *memory, const int freeMemory) {
    unsigned written = toString(start, memory, freeMemory);
    for (; start < end && freeMemory > written;) {
        memory[written++] = ' ';
        written += toString(++start, memory + written, static_cast<int>(freeMemory - written));
    }
}
