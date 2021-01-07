#pragma once
#include <stdio.h>
#include <string.h>
#include <vector>
#include <random>
#include "single_instance.hpp"
class math_utility {
public:
    void get_uniform_random_values(int total, int range, std::vector<int>&rands) {
        rands.clear();
        if (total <= 0 || range <= 0) {
            return;
        }
        std::random_device seed;
        std::mt19937 engine(seed());
        std::uniform_int_distribution<>uniform_dist(0, range);
        rands.resize(total);
        for (auto &r : rands) {
            r = uniform_dist(engine);
        }
    }
};

#define G_MATH_UTILITY single_instance<math_utility>::instance()