#include <iostream>
#include "file_utility.hpp"
int main() {
    G_FILE_UTILITY.format_printf("%s %d %d", "Hello format printf", 18, 13);

    return 0;
}