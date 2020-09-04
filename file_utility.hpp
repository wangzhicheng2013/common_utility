#pragma once
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <iostream>
#include <thread>
#include "string_utility.hpp"
class file_utility {
public:
    bool sed_file(const char *file_path, const char *pattern, const char *val) {
        struct stat st = { 0 };
        if (lstat(file_path, &st) < 0) {
            return false;
        }
        int fd = open(file_path, O_RDWR);
        if (fd < 0) {
            return false;
        }
        size_t file_size = st.st_size;
        char *mmap_ptr = (char *)mmap(nullptr, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if ((nullptr == mmap_ptr ) || ((char *)-1 == mmap_ptr)) {
            close(fd);
            return false;
        }
        std::string str(mmap_ptr, file_size);
        G_STRING_UTILITY.replace_all(str, pattern, val);
        memcpy(mmap_ptr, str.c_str(), str.size());
        munmap(mmap_ptr, file_size);
        close(fd);
        return true;
    }
    void get_cpu_model(std::string &model) {
        const char *shell_cmd = "cat /proc/cpuinfo | grep \"model name\" | cut -d ':' -f 2";
        FILE *fp = popen(shell_cmd, "r");
        if (!fp) {
            return;
        }
        char buf[128] = "";
        fgets(buf, sizeof(buf), fp);
        if (' ' == buf[0]) {
            model.assign(buf + 1, strlen(buf));
        }
        else {
            model = buf;
        }
        fclose(fp);
    }
    #define LOG(LEVEL, STR) log(LEVEL, __FILE__, __func__, __LINE__, STR)
    void log(const char *level, const char *file, const char *function, uint32_t line, const char *str) {
        std::cout << time(NULL) << "|" << std::this_thread::get_id() << "|" << level 
            << "|" << file << "|" << function << "|" << line << "|" << str << std::endl;
    }
};

#define  G_FILE_UTILITY single_instance<file_utility>::instance()