#include <iostream>
#include "ports_storage_utility.hpp"
int main() {
    G_PORTS_STORAGE_UTILITY.store(2889);
    G_PORTS_STORAGE_UTILITY.store(78);
    G_PORTS_STORAGE_UTILITY.store(3);
    std::cout << G_PORTS_STORAGE_UTILITY.existed(2889) << std::endl;
    std::cout << G_PORTS_STORAGE_UTILITY.existed(78) << std::endl;
    std::cout << G_PORTS_STORAGE_UTILITY.existed(3) << std::endl;
    std::cout << G_PORTS_STORAGE_UTILITY.existed(300) << std::endl;

    return 0;
}