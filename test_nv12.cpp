#include "arcsoft_nv12_blackening.h"
#include <iostream>
#include <string>
#include <fstream>
enum FILE_WRITE_MODE {
    OVER_WRITE,
    APPEND_WRITE
};
inline bool get_file_content(const char *path, std::string &str) {
    std::ifstream ifs(path, std::ios::in);
    if (!ifs || !ifs.good()) {
        return false;
    }
    str.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();
    return !str.empty();
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
int main() {
    std::string str;
    if (get_file_content("./1653210331_1920x1080_56029_33_1920x1080.NV12", str)) {
        size_t size = str.size();
        std::cout << "load success! size:" << size << std::endl;
        unsigned char *ppu8Plane0 = (unsigned char *)malloc(1920 * 1080 * 3 / 2);
        unsigned char *ppu8Plane1 = ppu8Plane0 + 1920 * 1080;
        memcpy(ppu8Plane0, str.c_str(), size);
        blackening_nv12(0, 0, 1920 / 2 - 1, 1080 - 1, 1920, 1080, ppu8Plane0, ppu8Plane1);
        size = 1920 * 1080 * 3 / 2;
        if (write_file_content("./11_1920x1080.NV12", (char *)ppu8Plane0, size, OVER_WRITE)) {
            std::cout << "rewrite success! size:" << size << std::endl;
        }
    }

    return 0;
}