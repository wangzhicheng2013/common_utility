#include <iostream>
#include "file_utility.hpp"
int main() {
	std::cout << G_FILE_UTILITY.file_is_stready("./test.cpp") << std::endl;
    return 0;
}