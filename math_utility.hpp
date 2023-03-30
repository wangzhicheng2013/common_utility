#pragma once
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <set>
#include <random>
#include <cmath>
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
    const char *base_conv(unsigned int num, unsigned short base) {
        static char ret_buf[sizeof(int) * CHAR_BIT + 1];
        if (base < 2 || base > 16) {
            return nullptr;
        }
        char *p = &ret_buf[sizeof(ret_buf) - 1];
        *p = 0;
        do {
            *--p = "0123456789ABCDEF"[num % base];
            num /= base;
        } while (num);
        return p;
    }
    unsigned short interger_has_one_count(unsigned int num) {
        const static unsigned short BIT_COUNTS[] = {
            0,          // 0000 -> 0个1  0索引号   
            1,          // 0001 -> 1个1  1索引号   
            1,          // 0010 -> 1个1  2索引号   
            2,          // 0011 -> 2个1  3索引号   
            1,          // 0100 -> 1个1  4索引号   
            2,          // 0101 -> 2个1  5索引号
            2,          // 0110 -> 2个1  6索引号
            3,          // 0111 -> 3个1  7索引号
            1,          // 1000 -> 1个1  8索引号
            2,          // 1001 -> 2个1  9索引号
            2,          // 1011 -> 3个1  10索引号
            3,          // 1011 -> 3个1  11索引号
            2,          // 1100 -> 2个1  12索引号
            3,          // 1101 -> 3个1  13索引号
            3,          // 1110 -> 3个1  14索引号
            4           // 1111 -> 4个1  15索引号
        };
        unsigned short one_count = 0;
        while (num) {
            one_count += BIT_COUNTS[num & 0x0f];
            num >>= 4;
        }
        return one_count;
    }
    double output_circular_constant() {
        double base = 1;
        double sum = 0;
        double pre_sum = 0;
        double sign = 1;
        while (true) {
            pre_sum = sum;
            sum += sign / base;
            if (fabs(pre_sum -  sum) <= 0.00000001) {
                return 4 * sum;
            }
            sign *= (-1);
            base += 2;
        }
        return 0;
    }
    void full_arrange(int index, bool *hash_table, int *array, int n) {
        static int count = 0;
        if (index == n) {
            std::cout << "----------" << ++count << "------------" << std::endl;
            for (int i = 0;i < n;i++) {
                std::cout << array[i] << " ";
            }
            std::cout << std::endl;
            return;
        }
        for (int i = 0;i < n;i++) {
            if (true == hash_table[i]) {
                continue;
            }
            array[index] = i;
            hash_table[i] = true;
            full_arrange(index + 1, hash_table, array, n);
            hash_table[i] = false;
        }
    }
    void full_arrange(int n) {
        const int max_n = 10;
        if (n > max_n) {
            std::cerr << "max n is:" << max_n << std::endl;
            return;
        }
        bool *hash_table = new bool[n] { 0 };
        if (!hash_table) {
            std::cerr << "new failed!" << std::endl;
            return;
        }
        int *array = new int[n] { 0 };
        if (!array) {
            free(hash_table);
            std::cerr << "new failed!" << std::endl;
            return;
        }
        full_arrange(0, hash_table, array, n);
        free(hash_table);
        free(array);
    }
    double square_root(double a) {
        if (a <= 0) {
            return 0;
        }
        double x1 = 1.0;
        double x2 = (x1 + a / x1) / 2;
        while (fabs(x2 - x1) > 0.00001) {
            x1 = x2;
            x2 = (x1 + a / x1) / 2;
        }
        return x2;
    }
    double positive_cubic_root(double a) {
        if (a <= 0) {
            return 0;
        }
        double x1 = a;
        double x2 = 1.0 / 3 * (2 * x1 + a / (x1 * x1));
        while (fabs(x1 - x2) > 0.00001) {
            x1 = x2;
            x2 = 1.0 / 3 * (2 * x1 + a / (x1 * x1));
        }
        return x2;
    }
    // a0x^n + a1x^(n-1) + ... + an
    double sum_of_polynomials(double x, const std::vector<double>&coefficient_array, int n) {
        assert((n + 1 == coefficient_array.size()) && (n > 0));
        double sum_pre = coefficient_array[0];
        double sum = 0;
        for (int i = 1;i <= n;i++) {
            sum = coefficient_array[i] + x * sum_pre;
            sum_pre = sum;
        }
        return sum;
    }
    // array such as [1;2,2,2,2,2]
    double continued_fraction_calculation(const std::vector<int>&array) {
        int p_2 = 0, p_1 = 1;
        int q_2 = 1, q_1 = 0;
        int p = 0, q = 0;
        for (auto &a : array) {
            p = a * p_1 + p_2;
            q = a * q_1 + q_2;
            p_2 = p_1;
            p_1 = p;
            q_2 = q_1;
            q_1 = q;
        }
        return 1.0 * p / q;
    }
    std::string get_significant_number(const std::string &str) {
        if (str.empty()) {
            return "";
        }
        auto pos = str.find('E');
        if (pos == std::string::npos) {
            pos = str.find('e');
        }
        if (pos == std::string::npos) {
            return "";
        }
        std::string digital_str(str, 0, pos);
        double digital = atof(digital_str.c_str());
        if (digital >= 10 || digital <= -10) {
            return "";
        }
        std::string index_str(str, pos + 1, str.size() - pos - 1);
        if (index_str.empty()) {
            return "";
        }
        int index = atoi(index_str.c_str());
        if (0 == index) {
            return digital_str;
        }
        if (!isdigit(digital_str[0])) {     // erase + or -
            digital_str.erase(0, 1);
        }
        digital_str.erase(1, 1);            // erase .
        std::string res;
        if (digital < 0) {
            res = '-';
        }
        if (index < 0) {
            res += "0.";
            res += std::string(-index - 1, '0');
            res += digital_str;
        }
        else {
            if (index + 1 < digital_str.size()) {
                digital_str.insert(index + 1, ".");
            }
            res += digital_str;
            if (index + 1 > digital_str.size()) {
                res += std::string (index - (digital_str.size() - 1), '0');
            }
            int i = 1;
            while (res[i] && res[i + 1]) {
                if (('0' == res[i]) && (('0' == res[i + 1]) || isdigit(res[i + 1]))) {
                    res.erase(i, 1);
                }
                else {
                    break;
                }
            }
        }
        return res;
    }
    int greatest_common_divisor(uint a, uint b) {
        return (0 == b) ? a : greatest_common_divisor(b, a % b);
    }
    double sinX(double x) {
        static const double precision = 0.00001;
        double old_val = x;
        double new_val = 0;

        uint64_t last_denominator = 1;
        uint64_t new_denominator = 0;

        double last_numerator = x;
        double new_numerator = 0;
        double t = 0;
        int n = 1;
        int sign = -1;
        while (true) {
            new_denominator = last_denominator * (++n) * (++n);
            last_denominator = new_denominator;

            new_numerator = last_numerator * x * x;
            last_numerator = new_numerator;

            t = 1.0 * new_numerator / new_denominator;
            new_val = old_val + t * sign;
            if (fabs(t) <= precision) {
                return new_val;
            }
            old_val = new_val;
            sign *= -1;
        }
        assert(0);
    }
};
struct eight_empresses {
    void solution() {
        back_trace(0);
    }
    inline int get_solution_num() const {
        return solution_num;
    }
private:
    bool check(int row, int col) {
        for (int i = 0;i < row;i++) {
            if ((queen[i] == col) || (abs(row - i) == abs(col - queen[i]))) {
                return false;
            }
        }
        return true;
    }
    void back_trace(int row) {
        if (row == EIGHT) {
            for (int i = 0;i < EIGHT;i++) {
                std::cout << "(" << i << "," << queen[i] << ")" << std::endl;
            }
            ++solution_num;
            return;
        }
        for (int col = 0;col < EIGHT;col++) {
            if (check(row, col)) {
                queen[row] = col;
                back_trace(row + 1);
            }
        }
    }
private:
    static const int EIGHT = 8;
    int queen[EIGHT] = { 0 };
    int solution_num = 0;
};
#define G_MATH_UTILITY single_instance<math_utility>::instance()