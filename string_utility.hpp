#pragma once
#include <string.h>
#include <string>
#include "single_instance.hpp"
class string_utility {
public:
    void replace_all(std::string &origin_str, const char *pattern, const char *val) {
        size_t size = strlen(val);
        std::string::size_type pos = 0;
        while (pos != std::string::npos) {
            pos = origin_str.find(pattern, pos);
            if (std::string::npos == pos) {
                break;
            }
            origin_str.replace(pos, size, val);
            pos += size;
        }
    }
};

#define  G_STRING_UTILITY single_instance<string_utility>::instance()