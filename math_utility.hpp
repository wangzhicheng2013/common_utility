#pragma once
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <set>
#include <random>
#include <algorithm>
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
    // 2/10,3/10,4/10,5/10,6/10,7/10,8/10,9/10按照+或-进行组合形成累加和为1的可能性是多少种
    // 例如：2/10 + 3/10 + 5/10 + 8/10 + 9/10 - 4/10 - 6/10 - 7/10
    // positive_set为{ {2,3,5,8,9}, {2,3,4,5,6,7} }即目标解正数集合
    void get_percent_sum(std::set<std::vector<int> >&positive_set) {
        srand(time(0));
        static const int N = 8;
        static const int array[N] = { 2, 3, 4, 5, 6, 7, 8, 9 };
        int iteration_count = 1000000;  // 迭代100w次
        int sum = 0;
        std::vector<int>positive;
        std::set<int>index_set;
        for (int loop = 1;loop < iteration_count;++loop) {
            sum = 0;
            positive.clear();
            index_set.clear();
            for (int i = 1;i <= N;i++) {
                int index = -1;
                do {
                    index = rand() % N;
                } while (index_set.find(index) != end(index_set));
                if (27 == sum) {
                    sort(begin(positive), end(positive));
                    positive_set.insert(positive);
                    break;
                }
                else if (sum + array[index] <= 27) {
                    sum += array[index];
                    positive.push_back(array[index]);
                    index_set.insert(index);
                }
                else {
                    break;
                }
            }
        }
    }
    double double_difference(double a, double b) {
        double abs_a = abs(a);
        double abs_b = abs(b);
        double d = std::max(abs_a, abs_b);
        return (0.0 == d) ? 0.0 : abs(a - b) / d;
    }
    uint64_t get_fibonacci(unsigned int n, unsigned int max_n = 100) {
        if (0 == n) {
            return 0;
        }
        if (1 == n) {
            return 1;
        }
        if (n <= max_n) {
            return get_fibonacci(n - 1) + get_fibonacci(n - 2);
        }
        return 0;
    }
};

#define G_MATH_UTILITY single_instance<math_utility>::instance()