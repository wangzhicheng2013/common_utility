#include <iostream>
#include "encry_utility.hpp"
int main() {
    std::string str;
    G_ENCRY_UTILITY.make_md5_str("123", str);
    std::cout << str << std::endl;
    
    return 0;
}