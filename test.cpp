#include <iostream>
#include "math_utility.hpp"
int main() {
    std::set<std::vector<int> >positive_set;
    G_MATH_UTILITY.get_percent_sum(positive_set);
    for (auto &positive : positive_set) {
        for (auto &e : positive) {
            std::cout << e << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "solution size:" << positive_set.size() << std::endl;
    
    return 0;
}