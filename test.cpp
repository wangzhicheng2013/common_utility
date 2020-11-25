#include <iostream>
#include "string_utility.hpp"
int main() {
    std::unordered_map<std::string, std::string>args;
    const char *url = "http://www.123.com/jk/lll?key0=val0&key1=val1&key2=val2&keyXX0=val_0090*";
    G_STRING_UTILITY.get_url_args(url, args);
    for (auto &e : args) {
        std::cout << e.first << " " << e.second << std::endl;
    }
    
    return 0;
}