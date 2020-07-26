//
// Created by ajk on 26/07/20.
//

#ifndef TESTSUIT_ARRAYJOINER_H
#define TESTSUIT_ARRAYJOINER_H

#include <sys/types.h>

/**
 * Joins two arrays such that joined[-1] is the last element of first array
 * and indices of second starts from zero (just like normal array)
 * definition:
 *      first[n] => joined[n-len(first)]
 *      second[n] => joined[n]
 */

class ArrayJoiner {
private:
    const char *first;
    const char *second;
    const uint len1;
public:
    template<unsigned N>
    ArrayJoiner(const char (&arr1)[N], const char *arr2): first(arr1), second(arr2), len1(N) {}

    char operator[](uint index) {
        if (index < 0) return first[index + len1];
        else return second[index - len1];
    }
};


#endif //TESTSUIT_ARRAYJOINER_H