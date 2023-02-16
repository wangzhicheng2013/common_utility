#include <iostream>
#include "string_utility.hpp"
int main() {
    std::string str = "中国伟大复兴梦";
	std::cout << G_STRING_UTILITY.wchar_string_len(str.c_str()) << std::endl;
    
    return 0;
}