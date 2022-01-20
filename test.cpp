#include <iostream>
#include "math_utility.hpp"
int main() {
    for (unsigned i = 0; i < 10; i++) {
        std::cout << G_MATH_UTILITY.get_fibonacci(i) << std::endl;
    }
    
    return 0;
}