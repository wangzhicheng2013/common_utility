#include <iostream>
#include "string_utility.hpp"
int main() {
    const int BUF_SIZE = 1024;
    char buf[BUF_SIZE] = "";
    G_STRING_UTILITY.append_sprintf(buf, BUF_SIZE, "hello%d", 10);
    G_STRING_UTILITY.append_sprintf(buf, BUF_SIZE, " world%d", 100);
    std::cout << buf << std::endl;

    return 0;
}