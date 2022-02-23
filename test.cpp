#include <iostream>
#include "cpu_utility.hpp"
int main() {
    std::cout << G_CPU_UTILITY.cpu_is_little_endian() << std::endl;
    
    return 0;
}