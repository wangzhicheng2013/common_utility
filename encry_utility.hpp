#pragma once
#include <string.h>
#include <string>
#include <openssl/md5.h>
#include "single_instance.hpp"
class encry_utility {
public:
    void make_md5_str(const char *str, std::string &md5_str) {
        MD5_CTX ctx;
        unsigned char md[64] = { 0 };
        MD5_Init(&ctx);
        MD5_Update(&ctx, str, strlen(str));
        MD5_Final(md, &ctx);
        int len = strlen((const char *)md);
        char buf[8] = "";
        md5_str.clear();
        for (int i = 0;i < len;i++) {
            snprintf(buf, sizeof(buf), "%02x", md[i]);
            md5_str += buf;
        }
    }
};

#define  G_ENCRY_UTILITY single_instance<encry_utility>::instance()