#pragma once
#include <stdio.h>
#include <string>
#include <sched.h>
#include <thread>
#include "single_instance.hpp"
class cpu_utility {
public:
    inline bool bind_cpu(unsigned cpu_no) {
        cpu_set_t mask = { 0 };
        CPU_SET(cpu_no, &mask);
        return sched_setaffinity(0, sizeof(mask), &mask) >= 0;
    }
    inline bool bind_all_cpus() {
        bool succ = true;
        int num = get_cpu_num();
        cpu_set_t mask = { 0 };
        CPU_ZERO(&mask);
        for (int cpu_no = 0;cpu_no < num;cpu_no++) {
            CPU_SET(cpu_no, &mask);
            if (sched_setaffinity(0, sizeof(mask), &mask) < 0) {
                succ = false;
            }
        }
        return succ;
    }
    inline int get_cpu_num() {
        return std::thread::hardware_concurrency();
    }
    inline bool run_in_cpu(int cpu_no) {
        cpu_set_t get = { 0 };
        CPU_ZERO(&get);
        if (sched_getaffinity(0, sizeof(cpu_set_t), get) < 0) {
            return false;
        }
        return CPU_ISSET(cpu_no, &get);
    }
};

#define  G_CPU_UTILITY single_instance<cpu_utility>::instance()