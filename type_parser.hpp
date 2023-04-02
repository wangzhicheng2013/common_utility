#pragma once
#include <string>
#include <type_traits>
class type_parser {
private:
    std::string str_;
public:
    type_parser(std::string str) : str_(str) {
    }
    template <typename T>
    operator T() && {
        void *tmp = nullptr;
        if (std::is_integral<T>::value) {
            auto t = atoi(str_.data());
            tmp = &t;
        }
        else if (std::is_same<T, float>::value || std::is_same<T, double>::value) {
            auto t = atof(str_.data());
            tmp = &t;
        }
        else {
            tmp = &str_;
        }
        return *((T *)(tmp));
    }
};
// int a = type_parser("1111")
// double b = type_parser("23.333")
// std::string c = type_parser("ABC")