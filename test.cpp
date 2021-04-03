#include <iostream>
#include "string_utility.hpp"
int main() {
    int failed = 0;
    for (int i = 0;i < 100;i++) {
        std::string str;
        //if (G_STRING_UTILITY.make_random_string(32, str)) // 0.007s
        if (G_STRING_UTILITY.make_random_hex_string(32, str)) {     // 1.550s 
            std::cout << "str = " << str << std::endl;
            continue;
        }
        ++failed;
    }
    std::cout << "failed = " << failed << std::endl;
    
    return 0;
}