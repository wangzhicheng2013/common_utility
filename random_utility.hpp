#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "single_instance.hpp"
static const int BASE_NUM = 16807;
static const long MAX_NUM = RAND_MAX;
static const int Q = MAX_NUM / BASE_NUM;
static const int R = MAX_NUM % BASE_NUM;
static long seed = 1;
static const double PI = 3.141592654;
class random_utility {
public:
    long get_random() {
        long hi = seed / Q;
        long lo = seed % Q;
        long t = BASE_NUM * lo - R * hi;
        if (t > 0) {
            seed = t;
        }
        else {
            seed = t + MAX_NUM;
        }
        return seed;
    }
    long get_range_random(long range) {
        return get_random() / (RAND_MAX / range + 1);
    }
    double gaussrand() {
        static double u = 0, v = 0;
        static int phrase = 0;
        double z = 0;
        if (0 == phrase) {
            u = (rand() + 1.0) / (RAND_MAX + 2.0);
            v = rand() / (RAND_MAX + 1.0);
            z = sqrt(-2 * log(u)) * sin(2 * PI * v);
        }
        else {
            z = sqrt(-2 * log(u)) * cos(2 * PI * v);
        }
        phrase = 1 - phrase;
        return z;
    }
};

#define  G_RANDOM_UTILITY single_instance<random_utility>::instance()