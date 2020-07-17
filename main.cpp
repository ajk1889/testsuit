#include <iostream>
#include "Generator.h"
#include <string>
int main() {
    constexpr auto N = 50U;
    unsigned char arr[N];
    arr[N-1] = '\0';
    Generator::get(50,50+N-1, arr);
    std::cout<<arr<<std::endl;
    return 0;
}
