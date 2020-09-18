#pragma once
#include <stdio.h>
#include <string>
#include "single_instance.hpp"
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
};

#define  G_NET_UTILITY single_instance<net_utility>::instance()