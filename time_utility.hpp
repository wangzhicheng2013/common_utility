#pragma once
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <string>
#include "single_instance.hpp"
class time_utility {
public:
    inline void make_utc_date(long time_stamp, std::string &date_str, bool is_milli = true) {
        if (true == is_milli) {
            time_stamp /= 1000;
        }
        struct tm *ptm = gmtime(&time_stamp);
        char now_time[64] = "";
        strftime(now_time, sizeof(now_time), "%Y-%m-%d %H:%M:%S", ptm);
        date_str = now_time;
    }
    inline std::string get_cur_time(const char *time_format = "%Y-%m-%d %T") {
        struct timeval tv = { 0 };
        gettimeofday(&tv, nullptr);
        struct tm *ptm = localtime(&tv.tv_sec);
        char now_time[64] = "";
        strftime(now_time, sizeof(now_time), time_format, ptm);
        return now_time;
    }
};

#define  G_TIME_UTILITY single_instance<time_utility>::instance()