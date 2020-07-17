//
// Created by ajk on 08/07/20.
//

#ifndef TESTSUIT_PAIR_H
#define TESTSUIT_PAIR_H

template<typename F, typename S>
struct Pair{
    F first;
    S second;
    Pair(F a, S b){
        first = a;
        second = b;
    }
};
#endif //TESTSUIT_PAIR_H
