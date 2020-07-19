//
// Created by ajk on 08/07/20.
//

#include "Generator.h"

void Generator::get(Long start, Long end, char *memory) {
    if (start == 0) {
        auto totalSpaces = spacesBehind(end).first + 2;
        generateString(0, totalSpaces, memory, end);
    } else {
        auto spaces = spacesBehind(start);
        auto startSpaces = spaces.first + 1;
        auto startExtraChars = spaces.second;
        auto endSpaces = spacesBehind(end).first + 2;
        Long freeMemory = end - start;
        Long written;
        if (startExtraChars == 0) {
            *memory = ' ';
            written = 1 + toString(startSpaces, memory + 1, static_cast<uint>(freeMemory - 1));
        } else {
            auto digitsToWrite = lenDigits(startSpaces) - startExtraChars + 1;
            auto extractedDigits = startSpaces % pow(10, digitsToWrite);
            written = 0;
            for (int len = lenDigits(extractedDigits); len < digitsToWrite; ++len)
                memory[written++] = '0';
            written = toString(extractedDigits, memory + written, static_cast<uint>(freeMemory - written));
        }
        if (freeMemory > written)
            memory[written++] = ' ';

        generateString(startSpaces + 1, endSpaces,
                       memory + written, static_cast<uint>(freeMemory - written));
    }
}

inline uint Generator::toString(Long num, char *ptr, uint freeMemory) {
    if (num > 0) {
        uint charLen = lenDigits(num);
        while (charLen > freeMemory) {
            num /= 10;
            --charLen;
        }
        ptr += charLen;
        while (num) {
            *(--ptr) = static_cast<char>('0' + num % 10);
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

void Generator::generateString(Long start, Long end, char *memory, const int freeMemory) {
    unsigned written = toString(start, memory, freeMemory);
    for (; start < end && freeMemory > written;) {
        memory[written++] = ' ';
        written += toString(++start, memory + written, static_cast<int>(freeMemory - written));
    }
}
