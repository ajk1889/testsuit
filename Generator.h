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
    static Pair<Long,int> lowerDigitChangeIndex(Long index);
    static Pair<Long, int> spacesBehind(Long index);
    static uint toString(Long num, Char *ptr, int freeMemory);
    static void generateString(Long start, Long end, Char *memory, int freeMemory);
    static void get(Long start, Long end, Char *memory);

    inline static uint lenDigits(Long num){
        return static_cast<uint>(std::floor(std::log10(num)))+1;
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
