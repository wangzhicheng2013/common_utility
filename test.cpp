#include <iostream>
#include "file_utility.hpp"
int main() {
    std::cout << G_FILE_UTILITY.get_file_size("./test.cpp") << std::endl;
    std::cout << G_FILE_UTILITY.dir_existed("/home/wangbin/test/aa") << std::endl;
    
    return 0;
}