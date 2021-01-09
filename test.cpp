#include <iostream>
#include "file_utility.hpp"
int main() {
    std::cout << G_FILE_UTILITY.file_existed("./test.cpp") << std::endl;
    
    return 0;
}