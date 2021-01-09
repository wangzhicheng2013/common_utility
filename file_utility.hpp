#pragma once
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <iostream>
#include <thread>
#include <fstream>
#include "string_utility.hpp"
enum FILE_WRITE_MODE {
    OVER_WRITE,
    APPEND_WRITE
};
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
    bool write_file_content(const char *path, const char *buf, size_t size, unsigned mode) {
        if (!path || !buf) {
            return false;
        }
        std::ofstream  ofs;
        switch (mode)
        {
        case OVER_WRITE:
            ofs.open(path, std::ios::in | std::ios::trunc);
            if (!ofs.is_open()) {
                return false;
            }
            break;
        case APPEND_WRITE:
            ofs.open(path, std::ios::in | std::ios::app);
            if (!ofs.is_open()) {
                return false;
            }
            break;
        default:
            return false;
            break;
        }
        ofs.write(buf, size);
        ofs.close();
        return true;
    }
    inline bool get_file_content(const char *path, std::string &str) {
        std::ifstream ifs(path, std::ios::in);
        if (!ifs || !ifs.good()) {
            return false;
        }
        str.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        ifs.close();
        return !str.empty();
    }
    // iterms consist of item first -- configure key second configure value
    bool sed_configure_file(const char *path, const std::vector<std::pair<std::string, std::string>>&iterms, const char *restart_cmd) {
        if (!path || !restart_cmd || iterms.empty()) {
            return false;
        }
        char buf[256] = "";
        for (auto &iterm : iterms) {
            snprintf(buf, sizeof(buf), "sed -i '/^%s/c\\%s=%s' %s > /dev/null &", iterm.first.c_str(), iterm.first.c_str(), iterm.second.c_str(), path);
            if (system(buf)) {
                return false;
            }
            usleep(1000);   // let shell process to finish
        }
        return (0 == system(restart_cmd));
    }
    bool file_existed(const char *path) {
        if (!path) {
            return false;
        }
        return 0 == access(path, F_OK);
    }
};

#define  G_FILE_UTILITY single_instance<file_utility>::instance()