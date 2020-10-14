#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
#include "single_instance.hpp"
struct  netcard_info {
    std::string name;
    std::string ip;
};
class net_utility {
public:
    void get_mac_str(unsigned long mac_val, std::string &mac_str) {
        static const char *convert_format = "%02x:%02x:%02x:%02x:%02x:%02x";
        char buf[24] = "";
        snprintf(buf, sizeof(buf), convert_format, (mac_val >> 40) & 0xff,
                                                           (mac_val >> 32) & 0xff,
                                                           (mac_val >> 24) & 0xff,
                                                           (mac_val >> 16) & 0xff,
                                                           (mac_val >> 8) & 0xff,
                                                           mac_val & 0xff);
        mac_str = buf;
    }
    inline void get_ipv4_addr(uint32_t ip, std::string &addr) {
        char buf[64] = "";
        snprintf(buf, sizeof(buf), "%u.%u.%u.%u", ((ip >> 24) & 0xFF), 
                                                      ((ip >> 16) & 0xFF),
                                                      ((ip >> 8) & 0xFF), 
                                                      ((ip >> 0) & 0xFF));
        addr = buf;
    }
    bool get_netcard_info(std::vector<netcard_info>&infos) {
        int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock_fd < 0) {
            return false;
        }
        infos.clear();
        unsigned char buf[1024] = "";
        struct ifconf ifc = { 0 };
        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = (caddr_t)buf;
        ioctl(sock_fd, SIOCGIFCONF, &ifc);
        struct ifreq *ifr = (struct ifreq *)buf;
        int netcard_size = ifc.ifc_len / sizeof(struct ifreq);
        netcard_info info;
        for (int i = 0;i < netcard_size;i++) {
            info.name = ifr->ifr_name;
            info.ip = inet_ntoa(((struct sockaddr_in *)&(ifr->ifr_addr))->sin_addr);
            ++ifr;
            infos.emplace_back(info);
        }
        close(sock_fd);
        return !infos.empty();
    }
};

#define  G_NET_UTILITY single_instance<net_utility>::instance()