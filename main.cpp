#include <iostream>
#include "Generator.h"
#include <string>
int main() {
    constexpr auto N = 50U;
    unsigned char arr[N];
    arr[N-1] = '\0';
    Generator::generateString(50,100, arr, N-1);
    std::cout<<arr<<std::endl;
    return 0;
}
