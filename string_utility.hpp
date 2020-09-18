#pragma once
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <string>
#include <vector>
#include <unordered_map>
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
    void get_url_args(const char *url, std::unordered_map<std::string, std::string>&args) {
        const char *p = url;
        std::vector<std::string>keys;
        std::vector<std::string>values;
        while(*p) {
            if('?' == *p) {
                break;
            }
            p++;
        }
        if (0 == *p) {
            return;
        }
        char tmp[1024] = "";
        int loop = 0;
        bool Get = false;
        while (*p) {
            if (*(p + 1) && !Get) {
                sscanf(p + 1, "%[^= | &]", tmp);
                if (strcmp(tmp, "")) {
                    Get = true;
                    if (!loop) {
                        keys.emplace_back(tmp);
                    }
                    else {
                        values.emplace_back(tmp);
                    }
                }
            }
            p++;
            if (0 == *p) {
                break;
            }
            if (('=' == *p) || ('&' == *p)) {
                if ('=' == *p) {
                    loop = 1;
                }
                else {
                    loop = 0;
                }
                Get = false;
            }
        }
        if (keys.size() != values.size()) {
            return;
        }
        int size = keys.size();
        for (int i = 0;i < size;i++) {
            args[keys[i]] = values[i];
        }
    }
    void get_special_sub_str(const std::string &original_str, char ch, std::string &sub_str) {
        auto pos = original_str.find(ch);
        if (std::string::npos == pos)  {
            sub_str = original_str;
            return;
        }
        sub_str.assign(original_str, 0, pos);
    }
    inline std::string get_format_str(const char *format_str, ...) {
        va_list args;
        va_start(args, format_str);
        char buf[1024] = "";
        vsnprintf(buf, sizeof(buf), format_str, args);
        va_end(args);
        return buf;
    }
    void split_string(const char *str, char ch, std::vector<std::string>&vec) {
        vec.clear();
        std::string tmp;
        for (int i = 0;str[i];i++) {
            if (ch == str[i]) {
                if (!tmp.empty()) {
                    vec.emplace_back(tmp);
                    tmp.clear();
                }
                continue;
            }
            tmp += str[i];
        }
        if (!tmp.empty()) {
            vec.emplace_back(tmp);
        }
    }
    void split_string(const char *str, const char *delim, std::vector<std::string>&vec) {
        vec.clear();
        char *p = nullptr;
        char *ptr = strtok_r((char *)str, delim, &p);
        while (ptr) {
            vec.emplace_back(ptr);
            ptr = strtok_r(nullptr, delim, &p);
        }
    }
    inline void add_quotation(const std::string &str, std::string &new_str) {
        new_str = "\"";
        new_str += str;
        new_str += "\"";
    }
};

#define  G_STRING_UTILITY single_instance<string_utility>::instance()