#pragma once
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "single_instance.hpp"
class ports_storage_utility {
public:
    inline void store(uint32_t port) {
        ports[port_index(port)] |= port_map(port);
    }
    inline bool valid(uint32_t port) {
        return port > 0 && port < MAX_PORTS;
    }
    inline bool existed(uint32_t port) {
        return ports[port_index(port)] & port_map(port);
    }
    inline void clear() {
        memset(ports, 0, sizeof(ports));
    }
private:
    inline uint16_t port_index(uint32_t port) {
        return port / 8;
    }
    inline uint8_t port_map(uint32_t port) {
        return 1 << (port % 8);
    }
private:
    static const int MAX_PORTS = 65536;
    uint8_t ports[MAX_PORTS / 8] = { 0 };
};

#define  G_PORTS_STORAGE_UTILITY single_instance<ports_storage_utility>::instance()