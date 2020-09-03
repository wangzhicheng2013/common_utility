#pragma once
#include <string.h>
#include <string>
#include <openssl/md5.h>
#include "single_instance.hpp"
class encry_utility {
public:
    void make_md5_str(const char *str, std::string &md5_str) {
        unsigned char md[64] = { 0 };
        MD5((unsigned char *)str, strlen(str), md);
        char buf[64] = "";
        for (int i = 0;i < MD5_DIGEST_LENGTH;;i++) {
            sprintf(buf + i * 2, "%.2x", md[i]);
        }
        buf[2 * MD5_DIGEST_LENGTH] = '\0';
        md5_str.assign(buf);
    }
};

#define  G_ENCRY_UTILITY single_instance<encry_utility>::instance()