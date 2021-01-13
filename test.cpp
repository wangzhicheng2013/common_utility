#include <iostream>
#include "file_utility.hpp"
int main() {
    std::vector<std::string>vec;
    if (G_FILE_UTILITY.read_file_content_to_vector("./test.cpp", vec)) {
        for (auto &line : vec) {
            std::cout << line << std::endl;
        }
    }
    
    return 0;
}