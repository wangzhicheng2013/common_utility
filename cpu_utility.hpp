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
enum MB_ALIGN {
    MB8 = 8u,
    MB16 = 16u,
    MB32 = 32u,
};
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
        snprintf(cmd, sizeof(cmd), "top -bn 1 | grep %s | grep -v \"grep\"", process_name_key);
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
    inline void byte_alignment(int &num, MB_ALIGN mb) {
        num = ((num + mb - 1) / mb) * mb;
    }
    short get_bit_from_char(unsigned char ch, short bit_n) {
        union byte_t
        {
            struct bits_t 
            {
                unsigned char b0:1;
                unsigned char b1:1;
                unsigned char b2:1;
                unsigned char b3:1;
                unsigned char b4:1;
                unsigned char b5:1;
                unsigned char b6:1;
                unsigned char b7:1;
            } bits;
            unsigned char ch;   
        };
        byte_t bt;
        bt.ch = ch;
        switch (bit_n)
        {
        case 0:
            return bt.bits.b7;
            break;
        case 1:
            return bt.bits.b6;
            break;
        case 2:
            return bt.bits.b5;
            break;
        case 3:
            return bt.bits.b4;
            break;
        case 4:
            return bt.bits.b3;
            break;
        case 5:
            return bt.bits.b2;
            break;
        case 6:
            return bt.bits.b1;
            break;
        case 7:
            return bt.bits.b0;
            break;
        }
        return -1;
    }
    static int get_cpu_mhz(int cpu_no) {
        char cmd[128] = { 0 };
        char buf[256] = { 0 };
        FILE *fp = nullptr;
    #ifdef QNX_OS
        // 8295 qnx cpu as following
        /*
        Processor1: 1091556528 Octa Kryo-695 Gold 2092MHz FPU
        Processor2: 1091556528 Octa Kryo-695 Gold 2092MHz FPU
        Processor3: 1091556528 Octa Kryo-695 Gold 2092MHz FPU
        Processor4: 1091556528 Octa Kryo-695 Gold 2092MHz FPU
        Processor5: 1091556544 Octa Kryo-695 Gold Plus 2380MHz FPU
        Processor6: 1091556544 Octa Kryo-695 Gold Plus 2380MHz FPU
        Processor7: 1091556544 Octa Kryo-695 Gold Plus 2380MHz FPU
        Processor8: 1091556544 Octa Kryo-695 Gold Plus 2380MHz FPU
        */
        snprintf(cmd, sizeof(cmd), "pidin info | grep Processor%d | grep MHz", cpu_no + 1);
        fp = popen(cmd, "r");
        if (!fp) {
            return 0;
        }
        fgets(buf, sizeof(buf) - 1, fp);
        fclose(fp);
        const char* ptr = strstr(buf, "MHz");
        if (nullptr == ptr) {
            return 0;
        }
        while (*ptr != ' ') {
            if (ptr == buf) {
                return 0;
            }
            --ptr;
        }
        return atoi(ptr + 1);
    #elif defined(ANDROID_OS)
        snprintf(cmd, sizeof(cmd), "cat /sys/devices/system/cpu/cpu%d/cpufreq/scaling_cur_freq", cpu_no);
        fp = popen(cmd, "r");
        if (!fp) {
            return 0;
        }
        fgets(buf, sizeof(buf) - 1, fp);
        fclose(fp);
        return atoi(buf) / 1000;
    #else
        snprintf(cmd, sizeof(cmd), "cat /proc/cpuinfo | grep \"cpu MHz\" | sed -e 's/.*:[^0-9]//' | head -n %d | tail -n 1", cpu_no + 1);
        fp = popen(cmd, "r");
        if (!fp) {
            return 0;
        }
        fgets(buf, sizeof(buf) - 1, fp);
        fclose(fp);
        return atoi(buf);   
    #endif
        return 0;
    }
};

#define  G_CPU_UTILITY single_instance<cpu_utility>::instance()