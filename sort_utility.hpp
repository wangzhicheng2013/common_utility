#pragma once
#include <stdio.h>
#include <string.h>
enum HEAP_TYPE {
    BIG_HEAP,
    SMALL_HEAP,
};
enum SORT_TYPE {
    ASC,
    DESC,
};
template <class T>
class sort_utility {
public:
    void heap_sort(T arr[], int length, int type) {
        if (length <= 0 || ((BIG_HEAP != type && SMALL_HEAP != type))) {
            return;
        }
        int i = 0;
        for (i = length / 2 - 1;i >= 0;i--) {
            if (BIG_HEAP == type) {
                big_heap_adjust(arr, i, length);
                continue;
            }
            small_heap_adjust(arr, i, length);
        }
        for (i = length - 1;i > 0;--i) {
            std::swap(arr[i], arr[0]);
            if (BIG_HEAP == type) {
                big_heap_adjust(arr, 0, i);
                continue;
            }
            small_heap_adjust(arr, 0, i);
        }
    }
private:
    void big_heap_adjust(T arr[], int i, int length) {
        int child = 0;
        while (2 * i + 1 < length) {
            child = 2 * i + 1;
            if (child < length - 1 && arr[child + 1] > arr[child]) {    // 指向最大的孩子节点
                ++child;
            }
            if (arr[i] < arr[child]) {
                std::swap(arr[i], arr[child]);
            }
            else {
                break;
            }
            i = child;
        }
    }
    void small_heap_adjust(T arr[], int i, int length) {
        int child = 0;
        while (2 * i + 1 < length) {
            child = 2 * i + 1;
            if (child < length - 1 && arr[child + 1] < arr[child]) {    // 指向最大的孩子节点
                ++child;
            }
            if (arr[i] > arr[child]) {
                std::swap(arr[i], arr[child]);
            }
            else {
                break;
            }
            i = child;
        }
    }
};