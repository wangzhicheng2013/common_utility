#pragma once
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "single_instance.hpp"
class string_utility {
public:
    void replace_all(std::string &origin_str, const char *pattern, const char *val) {
        if (!pattern || !val) {
            return;
        }
        std::string::size_type pos = origin_str.find(pattern, 0);
        int pattern_len = strlen(pattern);
        int val_len = strlen(val);
        while (pos != std::string::npos) {
            origin_str.replace(pos, pattern_len, val);
            pos += val_len;
            pos = origin_str.find(pattern, pos);
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
    void trim(std::string &str) {
        auto first = begin(str);
        auto last = end(str);
        while (first != last) {
            if ((' ' == *first) || ('\t' == *first)) {
                ++first;
            }
            else {
                break;
            }
        }
        while (first != last) {
            if ((' ' == *(last - 1)) || ('\t' == *(last - 1))) {
                --last;
            }
            else {
                break;
            }
        }
        str = str.assign(first, last);
    }
    bool make_random_string(size_t len, std::string &str) {
        if (len > 1024) {
            return false;
        }
        FILE *fp = fopen("/dev/urandom", "rb");
        if (!fp) {
            return false;
        }
        char *tmp_str = new char[len + 1];
        if (!tmp_str) {
            fclose(fp);
            return false;
        }
        memset(tmp_str, 0, len + 1);
        bool succ = true;
        if (len != fread(tmp_str, sizeof(char), len, fp)) {
            succ = false;
        }
        fclose(fp);
        if (succ) {
            str = tmp_str;
        }
        delete []tmp_str;
        return succ;
    }
    bool make_random_hex_string(size_t len, std::string &str) {
        if (len > 1024) {
            return false;
        }
        char cmd[64] = "";
        snprintf(cmd, sizeof(cmd), "openssl rand -hex %u", len);
        FILE *fp = popen(cmd, "r");
        if (!fp) {
            return false;
        }
        char *tmp_str = new char[len + 1];
        if (!tmp_str) {
            pclose(fp);
            return false;
        }
        memset(tmp_str, 0, len + 1);
        bool succ = true;
        if (!fgets(tmp_str, len, fp)) {
            succ = false;
        }
        pclose(fp);
        if (succ) {
            str = tmp_str;
        }
        delete []tmp_str;
        return succ;
    }
};

#define  G_STRING_UTILITY single_instance<string_utility>::instance()