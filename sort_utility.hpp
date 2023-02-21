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
    T find_kth_largest(vector<T>&nums, int k) {
        return find_the_largest(nums, 0, nums.size() - 1, k);
    }
    int quick_partition(vector<T>&nums, int start, int end) {
        int i = start, j = end;
        T tmp = nums[i];
        while (i < j) {
            while (i < j && nums[j] > tmp) {
                j--;
            }
            if (i < j) {
                nums[i++] = nums[j];
            }
            while (i < j && nums[i] < tmp) {
                i++;
            }
            if (i < j) {
                nums[j--] = nums[i];
            }
        }
        nums[i] = tmp;
        return i;
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
    T find_the_largest(vector<T>&nums, int start, int end, int k) {
        int pos = quick_partition(nums, start, end);
        int largest_index = end - pos + 1;
        if (k == largest_index) {
            return nums[pos];
        }
        else if (k < largest_index) {
            return find_the_largest(nums, pos + 1, end, k);
        }
        else {
            return find_the_largest(nums, start, pos - 1, k - (end - pos) - 1);
        }
    }
};