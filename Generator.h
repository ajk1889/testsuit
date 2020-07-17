//
// Created by ajk on 08/07/20.
//

#ifndef TESTSUIT_GENERATOR_H
#define TESTSUIT_GENERATOR_H
#include "Pair.h"
#include <memory>
#include <cmath>
#include <iostream>

typedef long long Long;
typedef unsigned char Char;

struct Generator {
    /**
     * @return Pair of "the index at which number's digit changes; Eg: 9->10, 99->100"
     * and "digits in number just before change" for above Eg. digits = 1, 2
     */
    static Pair<Long,int> lowerDigitChangeIndex(Long index) {
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
    /**
     * @return Pair of "the number of space characters behind given index"
     * and "number extra characters from last space, if char in given index is not space"
     */
    static Pair<Long, int> spacesBehind(Long index) {
        auto digitChangeIndex = lowerDigitChangeIndex(index);
        auto lowIndex = digitChangeIndex.first;
        auto digits = digitChangeIndex.second;
        auto countOfLowerDigitNumbers = max(0L, pow(10L, digits - 1) - 1);
        auto spaces = countOfLowerDigitNumbers + ((index - lowIndex) / (digits + 1));
        auto extraChars = (index - lowIndex) % (digits + 1);
        return {spaces, static_cast<int>(extraChars)};
    }
    static uint toString(Long num, Char *ptr, int freeMemory){
        if (num>0){
            uint charLen = static_cast<uint>(std::floor(std::log10(num)))+1;
            while (charLen>freeMemory){
                num/=10;
                --charLen;
            }
            uint written = charLen;
            while (num){
                ptr[--charLen] = '0' + num%10;
                num/=10;
            }
            return written;
        } else if (freeMemory>0){
            *ptr='0';
            return 1;
        } else return 0;
    }

    static void generateString(Long start, Long end, Char *memory, const int N){
        unsigned written = toString(start, memory, N);
        for (;start < end && N>written;) {
            memory[written++] = ' ';
            written += toString(++start, memory + written, static_cast<int>(N - written));
        }
    }

    static void get(Long start, Long end, Char *memory){
        std::allocator<Char> allocator;
        if (start==end) {
            return;
        } else if (start==0) {
            auto totalSpaces = spacesBehind(end).first + 2;
            generateString(0, totalSpaces, memory, end);
        } else {
            auto spaces = spacesBehind(start);
            auto startSpaces = spaces.first + 1;
            auto startExtraChars = spaces.second;
            auto endSpaces = spacesBehind(end).first + 1;
            generateString(startSpaces+1,endSpaces+1,
                    memory+startExtraChars,
                    static_cast<uint>(end - start));
            toString(startSpaces, memory, startExtraChars);
        }
    }
    inline static Long pow(Long base, unsigned long long exp){
        Long ans = 1;
        exp+=2;
        while (--exp > 1) ans*=base;
        return ans;
    }
    inline static Long max(Long a, Long b){
        return a>b?a:b;
    }
public:
    template<unsigned N>void read(const Char (&buffer)[N]){

    }
};


#endif //TESTSUIT_GENERATOR_H
