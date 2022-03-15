#include <iostream>
#include "sort_utility.hpp"
int main() {
    int arr[] = { 9, 10, 3, -1, 6 };
    G_SORT_UTILITY<int>.heap_sort(arr, sizeof(arr) / sizeof(int), SMALL_HEAP);
    for (auto i : arr) {
        std::cout << i << std::endl;
    }
    
    return 0;
}