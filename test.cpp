#include <iostream>
#include "time_utility.hpp"
int main() {
    int day = 0;
    int week = 0;
    int hour = 0;
    G_TIME_UTILITY.get_gmt_current_day_week_hour(day, week, hour);
    std::cout << "day = " << day << std::endl;
    std::cout << "week = " << week << std::endl;
    std::cout << "hour = " << hour << std::endl;
    
    return 0;
}