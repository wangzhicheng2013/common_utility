#include <iostream>
#include "math_utility.hpp"
int main() {
    std::vector<double>coefficient_array = { 7, 2, 3 };
    std::cout << G_MATH_UTILITY.sum_of_polynomials(1.0, coefficient_array, 2) << std::endl;

    return 0;
}