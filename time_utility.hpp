#pragma once
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <string>
#include <vector>
#include <chrono>
#include "single_instance.hpp"
enum CLOCK_TYPE {
    SYSTEM_CLOCK,
    STEADY_CLOCK
};
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
    inline void get_gmt_current_day_week_hour(int &day, int &week, int &hour) {
        time_t now = time(0);
        now += 28800;
        struct tm today = { 0 };
        gmtime_r(&now, &today);
        day = today.tm_mday;
        week = today.tm_wday;
        if (0 == week) {
            week = 7;
        }
        hour = today.tm_hour;
    }
    inline int64_t get_now_microsecs(CLOCK_TYPE type = SYSTEM_CLOCK) {
        if (SYSTEM_CLOCK == type) {
            auto now = std::chrono::system_clock::now();
            auto now_tp = std::chrono::time_point_cast<std::chrono::microseconds>(now);
            auto us = now_tp.time_since_epoch().count();
            return us;
        }
        else if (STEADY_CLOCK == type) {
            auto now = std::chrono::steady_clock::now();
            auto now_tp = std::chrono::time_point_cast<std::chrono::microseconds>(now);
            auto us = now_tp.time_since_epoch().count();
            return us;
        }
        return -1;
    }
    inline int64_t get_now_millsecs(CLOCK_TYPE type = SYSTEM_CLOCK) {
        if (SYSTEM_CLOCK == type) {
            auto now = std::chrono::system_clock::now();
            auto now_tp = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
            auto us = now_tp.time_since_epoch().count();
            return us;
        }
        else if (STEADY_CLOCK == type) {
            auto now = std::chrono::steady_clock::now();
            auto now_tp = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
            auto us = now_tp.time_since_epoch().count();
            return us;
        }
        return -1;
    }
};

#define  G_TIME_UTILITY single_instance<time_utility>::instance()