#pragma once
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdarg.h>
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
    size_t get_file_size(const char *path) {
        if (!path) {
            return 0;
        }
        if (!file_existed(path)) {
            return 0;
        }
        struct stat fs = { 0 };
        if (lstat(path, &fs) < 0) {
            return 0;
        }
        return fs.st_size;
    }
    bool dir_existed(const char *path) {
        if (!path) {
            return false;
        }
        struct stat dir_stat = { 0 };
        if (stat(path, &dir_stat) < 0) {
            return false;
        }
        return S_ISDIR(dir_stat.st_mode);
    }
    bool create_dir(const char *path) {
        if (!path) {
            return false;
        }
        char cmd[128] = "";
        snprintf(cmd, sizeof(cmd), "mkdir -p %s", path);
        int ret = system(cmd);
        usleep(100);
        return (0 == ret) && dir_existed(path);
    }
    bool read_file_content_to_vector(const char *path, std::vector<std::string>&vec) {
        std::ifstream ifs(path, std::ios::in);
        if (!ifs || !ifs.is_open()) {
            return false;
        }
        vec.clear();
        std::string line;
        while (getline(ifs, line)) {
            if (line.empty()) {
                continue;
            }
            vec.emplace_back(line);
        }
        ifs.close();
        return true;
    }
    bool change_pcapng_to_pcap(const char *pcapng_file_path, std::string &pcap_file_path) {
        const char *pos = strstr(pcapng_file_path, ".pcapng");
        if (!pos) {
            return false;
        }
        size_t len = pos - pcapng_file_path;
        pcap_file_path.assign(pcapng_file_path, len);
        pcap_file_path += ".pcap";
        std::string cmd = "tcpdump -r ";
        cmd += pcapng_file_path;
        cmd += " -w ";
        cmd += pcap_file_path;
        cmd += " > /dev/null 2>&1";
        int ret = system(cmd.c_str());
        usleep(100);
        return 0 == ret;
    }
    bool delete_file(const char *file_path) {
        return 0 == unlink(file_path);
    }
	inline long get_last_change_time_for_file(const char *path) {
		struct stat fs = { 0 };
		if (lstat(path, &fs) < 0) {
			return -1;
		}
		return fs.st_ctime;
	}
    inline bool file_is_stready(const char *path) {
        long t = get_last_change_time_for_file(path);
        if (t < 0) {
            return false;
        }
        return (time(nullptr) - t) >= 3;
    }
    void format_printf(const char *fmt, ...) {
        va_list argp;
        va_start(argp, fmt);
        vfprintf(stdout, fmt, argp);
        va_end(argp);
        printf("\n");
    }
    // blacken the designated area of nv12 image
    // left:Abscissa of upper left corner of rectangular area
    // top:Vertical coordinate of upper left corner of rectangular area
    // right:Abscissa of lower right corner of rectangular area
    // bottom:Vertical coordinate of the lower right corner of the rectangular area
    // width:Original image width
    // height:Original image height
    // ppu8Plane_y:Y channel address
    // ppu8Plane_uv:UV channel address
    void blackening_nv12(int left, 
                        int top,
                        int right, 
                        int bottom,
                        int width,
                        int height,
                        unsigned char *ppu8Plane_y,
                        unsigned char *ppu8Plane_uv) {
        if (!ppu8Plane_y || !ppu8Plane_uv) {
            return;
        }
        if (!fix(left, top, right, bottom, width, height)) {
            return;
        }
        int w = right - left;
        for (int j = top;j <= bottom;j++) {
            memset(ppu8Plane_y + left + j * width, 0, w);
            memset(ppu8Plane_uv + left + j / 2 * width, 128, w);
        }
    }
private:
    // Correction of abscissa according to width
    void fix_x(int &x, int width) {
        if (x < 0) {
            x = 0;
        }
        else if (x >= width) {
            x = width - 1;
        }
    }
    void fix_y(int &y, int height) {
        if (y < 0) {
            y = 0;
        }
        else if (y >= height) {
            y = height - 1;
        }    
    }
    // Correct rectangular area according to image width and height
    bool fix(int &left, int &top, int &right, int &bottom, int width, int height) {
        fix_x(left, width);
        fix_x(right, width);

        fix_y(top, height);
        fix_y(bottom, height);
        int w = right - left;
        int h = bottom - top;
        if (w <= 0 || h <= 0) {
            return false;
        }
        return true;    
    }
};

#define  G_FILE_UTILITY single_instance<file_utility>::instance()