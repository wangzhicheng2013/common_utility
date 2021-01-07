#include <iostream>
#include "math_utility.hpp"
int main() {
    std::vector<int>rands;
    G_MATH_UTILITY.get_uniform_random_values(100, 99, rands);
    for (auto r : rands) {
        std::cout << r << std::endl;
    }
    
    return 0;
}