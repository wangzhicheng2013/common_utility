#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "single_instance.hpp"
static const int BASE_NUM = 16807;
static const long MAX_NUM = RAND_MAX;
static const int Q = MAX_NUM / BASE_NUM;
static const int R = MAX_NUM % BASE_NUM;
static long seed = 1;
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
};

#define  G_RANDOM_UTILITY single_instance<random_utility>::instance()