#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/time.h>
#include <string>
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
        if (sched_getaffinity(0, sizeof(cpu_set_t), &get) < 0) {
            return false;
        }
        return CPU_ISSET(cpu_no, &get);
    }
	bool cpu_is_little_endian() {
		static const union {
			int a;
			char b;
		} tmp { 1 } ;
		return 1 == tmp.b;
	}
    bool get_top_info_by_process(const char *process_name_key, int field_index, char *info) {
        char cmd[128] = { 0 };
        snprintf(cmd, sizeof(cmd), "top -bn 1 | grep %s", process_name_key);
        FILE *fp = popen(cmd, "r");
        if (!fp) {
            return false;
        }
        char buf[256] = { 0 };
        fgets(buf, sizeof(buf) - 1, fp);
        fclose(fp);
        int index = 0;
        int i = 0;
        int len = 0;
        const char *p = buf;
        while (buf[i]) {
            if (buf[i] != ' ' && (' ' == buf[i + 1])) {
                index++;
            }
            if (field_index == index) {
                len = buf + i - p + 1;
                if (len < 64) {
                    memcpy(info, p, len);
                }
                else {
                    memcpy(info, p, 63);
                }
                return true;
            }
            if ((buf[i] != ' ') && (' ' == buf[i - 1])) {
                p = buf + i;
            }
            i++;
        }
        len = buf + i - p;
        if (len < 64) {
            memcpy(info, p, len);
        }
        else {
            memcpy(info, p, 63);
        }
        return true;
    }
    double get_cpu_usage(const char *process_name_key) {
        char info[64] = { 0 };
        static const int CPU_INDEX_IN_TOP = 9;
        if (true == get_top_info_by_process(process_name_key, CPU_INDEX_IN_TOP, info)) {
            return atof(info);
        }
        return 0;
    }
};

#define  G_CPU_UTILITY single_instance<cpu_utility>::instance()