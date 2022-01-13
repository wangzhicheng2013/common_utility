#include <iostream>
#include "random_utility.hpp"
int main() {
    int n = 100;
    for (int i = 0;i < n;i++) {
        std::cout << G_RANDOM_UTILITY.gaussrand() << std::endl;    
    }

    return 0;
}