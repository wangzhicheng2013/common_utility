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
        snprintf(cmd, sizeof(cmd), "openssl rand -hex %zd", len);
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
    // conf_args: { tcp:11731,11732 udp:123,9900,9001 }
    // tcp_ports: [11731,11732]
    // tcp_num: 2
    // udp_ports: [123,9000,9001]
    // udp_num: 3
    void draw_tcp_udp_ports_from_args(const char *conf_args,
                                     int *tcp_ports,
                                     int *tcp_num, 
                                     int *udp_ports,
                                     int *udp_num) {
        if (!conf_args || !tcp_ports || !tcp_num || !udp_ports || !udp_num) {
            return;
        }
        char tcp_tmp[512] = "";
        char udp_tmp[512] = "";
        if (strstr(conf_args, "tcp:")) {
            sscanf(conf_args, "{ tcp:%s udp:%s }", tcp_tmp, udp_tmp);
        }
        else if (strstr(conf_args, "udp:")) {
            sscanf(conf_args, "{ udp:%s }", udp_tmp);
        }
        *tcp_num = 0;
        *udp_num = 0;
        if (!strlen(tcp_tmp) && !strlen(udp_tmp)) {
            return;
        }
        char *token = NULL;
        char *save = NULL;
        char delim[4] = ", ";
        if (strlen(tcp_tmp) > 0) {
            // seperate tcp ports
            token = strtok_r(tcp_tmp, delim, &save);
            while (token) {
                tcp_ports[*tcp_num] = atoi(token);
                token = strtok_r(NULL, delim, &save);
                ++(*tcp_num);
            }
        }
        token = NULL;
        save = NULL;
        if (strlen(udp_tmp) > 0) {
            // seperate udp ports
            token = strtok_r(udp_tmp, delim, &save);
            while (token) {
                udp_ports[*udp_num] = atoi(token);
                token = strtok_r(NULL, delim, &save);
                ++(*udp_num);
            }
        }
    }
    inline void *safe_alloc(unsigned long size) {
        void *addr = calloc(size, sizeof(char));
        if (addr) {
            return addr;
        }
        return nullptr;
    }
    enum STRING_OPERATION_CODE {
        STRING_OPERATION_ERROR = -1,
        STRING_OPERATION_TRUNCATION = -2,
        STRING_OPERATION_OK = 0,
    };
    int safe_strncpy(const char *src, char *dst, size_t dst_size) {
        if (!src || !dst || !dst_size) {
            return STRING_OPERATION_ERROR;
        }
        dst[dst_size - 1] = 0;
        if (!strncpy(dst, src, dst_size)) {
            return STRING_OPERATION_ERROR;
        }
        if (dst[dst_size - 1]) {
            dst[dst_size - 1] = 0;
            return STRING_OPERATION_TRUNCATION;
        }
        return STRING_OPERATION_OK;
    }
    // dst_size不包括\0
    char *safe_strndup(const char *src, size_t dst_size) {
        size_t total_size = dst_size + 1;       // 留取\0
        char *dst = (char *)safe_alloc(total_size);       
        if (!dst) {
            return nullptr;
        }
        if (STRING_OPERATION_ERROR == safe_strncpy(src, dst, total_size)) {
            free(dst);
            return nullptr;
        }
        return dst;
    }
    char **extended_split_string(const char *str,
                          const char *sep_chars, 
                          int max_toks, 
                          int *num_toks, 
                          char meta_char) {
        if (!str || !strlen(str) || (sep_chars && !strlen(sep_chars))) {
            return nullptr;
        }
        static const char *white_space = " \t";
        if (!sep_chars) {
            sep_chars = white_space;
        }
        if (!check_sep_chars(sep_chars, meta_char)) {
            return nullptr;
        }
        int tok_start = find_first_token(str, sep_chars);
        if (-1 == tok_start) {
            return nullptr;
        }
        if (1 == max_toks) {
            return make_only_one_token(str, tok_start, num_toks);
        }
        bool escaped = false;
        static const int TOKS_BUF_SIZE = 100;
        static const int toks_buf_size_increment = 10;
        char *toks_buf[TOKS_BUF_SIZE] = { 0 };
        size_t toks_buf_size = TOKS_BUF_SIZE;
        char **toks_alloc = nullptr;
        char **toks = toks_buf;
        char **retstr = nullptr;
        size_t cur_tok = 0;
        int i = 0, j = 0;
        for (i = tok_start;str[i];i++) {
            if (escaped) {
                escaped = false;
                continue;
            }
            if (meta_char == str[i]) {
                escaped = true;
                continue;
            }
            if (!is_sep_char(sep_chars, str[i])) {
                continue;
            }
            // 开始切出token
            for (j = i;j > tok_start;j--) {
                if (!isspace(str[j - 1])) {
                    break;
                }
            }
            if (tok_start == j) {
                continue;
            }
            toks[cur_tok++] = copy_token(&str[tok_start], j - tok_start, sep_chars, meta_char);
            // 检测余下的字符串是否只含有分隔符或空格
            for (;str[i];i++) {
                if (!is_sep_char(sep_chars, str[i]) && !isspace(str[i])) {
                    break;
                }
            }
            if (!str[i]) {
                *num_toks = cur_tok;
                if (toks != toks_alloc) {
                    retstr = (char **)safe_alloc(sizeof(char *) * cur_tok);
                    memcpy(retstr, toks, sizeof(char *) * cur_tok);
                }
                else {
                    retstr = toks;
                }
                return retstr;
            }
            // 已达到token数 使用toks_alloc分配新内存 将旧内容（可能是toks_buf或toks_alloc）拷贝到toks_alloc
            if (toks_buf_size == cur_tok) {
                char **tmp = nullptr;
                if (toks_alloc) {
                    tmp = toks_alloc;
                }
                else {
                    tmp = toks_buf;
                }
                if (max_toks) {
                    toks_buf_size = max_toks;
                }
                else {
                    toks_buf_size = cur_tok + toks_buf_size_increment;
                }
                toks_alloc = (char **)safe_alloc(sizeof(char *) * toks_buf_size);
                memcpy(toks_alloc, tmp, sizeof(char *) * cur_tok);
                toks = toks_alloc;
                if (tmp != toks_buf) {
                    free(tmp);
                }
            }
            if (max_toks && (1 + cur_tok == max_toks)) {
                *num_toks = cur_tok + 1;
                if (toks != toks_alloc) {
                    retstr = (char **)safe_alloc(sizeof(char *) * (cur_tok + 1));
                    memcpy(retstr, toks, sizeof(char *) * (cur_tok + 1));
                }
                else {
                    retstr = toks;
                }
                for (int j = strlen(str);j > tok_start;j--) {
                    if (!isspace(str[j - 1])) {
                        break;
                    }
                }
                retstr[cur_tok] = safe_strndup(&str[i], j - i);
                if (!retstr[cur_tok]) {
                    extended_split_free(&retstr, cur_tok + 1);
                    return nullptr;
                }
                return retstr;
            }
            tok_start = i;
        }
        // str都是转义字符
        if (escaped) {
            for (i = 0;i < cur_tok;i++) {
                free(toks[i]);
            }
            if (toks == toks_alloc) {
                free(toks_alloc);
            }
            return nullptr;
        }
        for (j = i;j > tok_start;j--) {
            if (!isspace(str[j - 1])) {
                break;
            }
        }
        // 如果启用分配多余内存 则toks == toks_alloc
        if (toks != toks_alloc) {
            retstr = (char **)safe_alloc(sizeof(char *) * (cur_tok + 1));
            memcpy(retstr, toks, sizeof(char *) * (cur_tok + 1));
        }
        else {
            retstr = toks;
        }
        retstr[cur_tok] = copy_token(&str[tok_start], j - tok_start, sep_chars, meta_char);
        *num_toks = cur_tok + 1;
        return retstr;
    }
    void extended_split_free(char ***pbuf, int num_toks) {
        if (!pbuf || !(*pbuf)) {
            return;
        }
        char **buf = *pbuf;
        for (int i = 0;i < num_toks;i++) {
            if (buf[i]) {
                free(buf[i]);
                buf[i] = nullptr;

            }
        }
        free(buf);
        *pbuf = nullptr;
    }
    int str_len(const char *buf, int buf_size) {
        int i = 0;
        if (!buf || buf_size <= 0) {
            return STRING_OPERATION_ERROR;
        }
        for (;i < buf_size;i++) {
            if ('\0' == buf[i]) {
                break;
            }
        }
        if (i >= buf_size) {
            return STRING_OPERATION_ERROR;
        }
        return i;
    }
    int append_sprintf(char *buf, size_t buf_size, const char *format, ...) {
        if (!buf || !buf_size || !format) {
            return -1;
        }
        int string_len = str_len(buf, buf_size);
        if (STRING_OPERATION_ERROR == string_len) {
            buf[0] = '\0';
            string_len = 0;
        }
        buf[buf_size - 1] = '\0';
        va_list ap;
        va_start(ap, format);
        int ret = vsnprintf(buf + string_len, buf_size - string_len, format, ap);
        va_end(ap);
        if (ret < 0) {
            return STRING_OPERATION_ERROR;
        }
        if (buf[buf_size - 1] || (ret >= buf_size - string_len)) {
            buf[buf_size - 1] = '\0';
            return STRING_OPERATION_TRUNCATION;
        }
        return STRING_OPERATION_OK;
    }
	int pattern_matching(char *m_str, char *p_str) {
		int i = 0, j = 0;
        int k = 0;
        int count = 0;
        while (m_str[i]) {
            k = i;
            j = 0;
            while (p_str[j]) {
                if (p_str[j] != m_str[i]) {
                    break;
                }
                i++;
                j++;
            }
            if (!p_str[j]) {
                count++;
                k = i;
            }
            else {
                i++;
            }
        }
        return count;
    }
    int pattern_matching(const std::string &m_str, const std::string &p_str) {
        int count = 0;
        int pos = 0;
        while (true) {
            pos = m_str.find(p_str, pos);
            if (pos != std::string::npos) {
                ++count;
                pos++;
            }
            else {
                break;
            }
        }
        return count;
    }
    size_t wchar_string_len(const char *str) {
        wchar_t *wt = (wchar_t *)str;
        return wcslen(wt);
    }
    
    size_t get_string_characters(const char *str) {
        static const char *tmp = "我";
        static const size_t chinese_word_len = strlen(tmp);
        size_t len_non_chinese = 0;
        size_t len = 0;
        for (int i = 0;str[i];i++) {
            if ((str[i] <= 255) && (str[i] >= 0)) {
                len_non_chinese++;
            }
            else {
                ++len;
            }
        }
        len /= chinese_word_len;
        return len_non_chinese + len;
    }
private:
    bool is_sep_char(const char *sep_chars, char ch) {
        for (int i = 0;sep_chars[i];i++) {
            if (ch == sep_chars[i]) {
                return true;
            }
        }
        return false;
    }
    // abc=>abc
    // abc\taa=>abc\taa
    // abc\\a=>abc\a
    // abc\\\\a=>abc\\a
    // abc\\\ta=>abc\ta
    // abc\\ta=>abc\ta
    char *copy_token(const char *str, int len, const char *sep_chars, char meta_char) {
        bool is_meta = false;
        int token_len = 0;
        for (int i = 0;i < len;i++) {
            if (!is_meta) {
                if (meta_char == str[i]) {
                    is_meta = true;
                    continue;
                }
            }
            else {
                if (!is_sep_char(sep_chars, str[i])) {
                    ++token_len;
                }
                is_meta = false;
            }
            ++token_len;
        }
        char *token = (char *)safe_alloc(token_len + 1);
        if (!token) {
            return nullptr;
        }
        is_meta = false;
        int i = 0, j = 0;
        for (;i < len;i++) {
            if (!is_meta) {
                if (meta_char == str[i]) {
                    is_meta = true;
                    continue;
                }
            }
            else {
                if (!is_sep_char(sep_chars, str[i])) {
                    token[j++] = meta_char;
                }
                is_meta = false;
            }
            token[j++] = str[i];
        }
        return token;
    }
    bool check_sep_chars(const char *sep_chars, char meta_char) {
        for (int i = 0;sep_chars[i];i++) {
            if (meta_char == sep_chars[i]) {
                return false;
            }
        }
        return true;
    }
    int find_first_token(const char *str, const char *sep_chars) {
        for (int i = 0;str[i];i++) {
            if (!is_sep_char(sep_chars, str[i]) && !isspace(str[i])) {
                return i;
            }
        }
        return -1;
    }
    char **make_only_one_token(const char *str, int token_start, int *num_toks) {
        char **rest_str = (char **)safe_alloc(sizeof(char *));
        if (!rest_str) {
            return nullptr;
        }
        rest_str[0] = safe_strndup(&str[token_start], strlen(str) - token_start);
        if (!rest_str[0]) {
            extended_split_free(&rest_str, 1);
            return nullptr;
        }
        *num_toks = 1;
        return rest_str;
    }
    // s = "abcd" t = "abcde"
    char find_the_only_difference(std::string s, std::string t) {
        if (0 == s.size()) {
            return 0;
        }
        if (s.size() + 1 != t.size()) {
            return 0;
        }
        int i = 0;
        int size = s.size();
        char ch = 0;
        for (;i < size;i++) {
            ch = ch ^ s[i] ^ t[i];
        }
        return ch ^ t[i];
    }
};

#define  G_STRING_UTILITY single_instance<string_utility>::instance()