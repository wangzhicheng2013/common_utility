#include <iostream>
#include "string_utility.hpp"
int main() {
    std::string str = "a中国伟大复兴1梦1";
	std::cout << G_STRING_UTILITY.get_string_characters(str.c_str()) << std::endl;
    
    return 0;
}