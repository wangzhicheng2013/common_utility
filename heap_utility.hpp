#pragma once
#include <cassert>
#include <vector>
#include <functional>
#include <algorithm>
enum HEAP_TYPE {
    BIG_HEAP,
    SMALL_HEAP,
};
template <typename T>
bool lesser(const T& a, const T& b) {
    return a < b;
}
template <typename T>
bool greater(const T& a, const T& b) {
    return a >= b;
}
template <typename DataType, HEAP_TYPE flag>
class heap {
public:
    const std::function<bool(const DataType&, const DataType&)>cmp_fun = (BIG_HEAP == flag) ? lesser<DataType> : greater<DataType>;
private:
    std::vector<DataType>vec_;
public:
    heap() {
        assert((BIG_HEAP == flag) || (SMALL_HEAP == flag));
    }
    void add(const DataType& e) {
        vec_.emplace_back(e);
        std::push_heap(vec_.begin(), vec_.end(), cmp_fun);
    }
    inline DataType peek() {
        assert(vec_.size() >= 1);
        return vec_[0];
    }
    DataType poll() {
        assert(vec_.size() >= 1);
        auto e = vec_[0];
        std::pop_heap(vec_.begin(), vec_.end(), cmp_fun);
        vec_.pop_back();
        return e;
    }
};