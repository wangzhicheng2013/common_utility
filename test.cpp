#include <iostream>
#include "time_utility.hpp"
int main() {
	std::cout << G_TIME_UTILITY.get_now_microsecs() << std::endl;
	std::cout << G_TIME_UTILITY.get_now_millsecs() << std::endl;

    
    return 0;
}