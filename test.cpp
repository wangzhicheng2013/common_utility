#include <iostream>
#include "string_utility.hpp"
void show(int *tcp_ports, int *tcp_num, int *udp_ports, int *udp_num) {
    std::cout << "tcp ports" << std::endl;
    for (int i = 0;i < *tcp_num;i++) {
        std::cout << tcp_ports[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "udp ports" << std::endl;
    for (int i = 0;i < *udp_num;i++) {
        std::cout << udp_ports[i] << " ";
    }
    std::cout << std::endl;
}
int main() {
    const char *conf_args = "{ tcp:2333 udp:12 }";
    const char *conf_args1 = "{ tcp:2333,1222 udp:12 }";
    const char *conf_args2 = "{ tcp:2333,2222 }";
    const char *conf_args3 = "{ tcp:2333 }";
    const char *conf_args4 = "{ udp:12 }";
    const char *conf_args5 = "{ udp:1112,1222 }";
    const char *conf_args6 = "{ tcp:2333,1222,12211 udp:12,900,1 }";
    for (int i = 0;i < 7;i++) {
        int tcp_ports[100] = { 0 };
        int tcp_num = 0;
        int udp_ports[100] = { 0 };
        int udp_num = 0;
        switch(i) {
        case 0:
            G_STRING_UTILITY.draw_tcp_udp_ports_from_args(conf_args, tcp_ports, &tcp_num, udp_ports, &udp_num);
            show(tcp_ports, &tcp_num, udp_ports, &udp_num);
            break;
        case 1:
            G_STRING_UTILITY.draw_tcp_udp_ports_from_args(conf_args1, tcp_ports, &tcp_num, udp_ports, &udp_num);
            show(tcp_ports, &tcp_num, udp_ports, &udp_num);
            break;
        case 2:
            G_STRING_UTILITY.draw_tcp_udp_ports_from_args(conf_args2, tcp_ports, &tcp_num, udp_ports, &udp_num);
            show(tcp_ports, &tcp_num, udp_ports, &udp_num);
            break;
        case 3:
            G_STRING_UTILITY.draw_tcp_udp_ports_from_args(conf_args3, tcp_ports, &tcp_num, udp_ports, &udp_num);
            show(tcp_ports, &tcp_num, udp_ports, &udp_num);
            break;
        case 4:
            G_STRING_UTILITY.draw_tcp_udp_ports_from_args(conf_args4, tcp_ports, &tcp_num, udp_ports, &udp_num);
            show(tcp_ports, &tcp_num, udp_ports, &udp_num);
            break;
        case 5:
            G_STRING_UTILITY.draw_tcp_udp_ports_from_args(conf_args5, tcp_ports, &tcp_num, udp_ports, &udp_num);
            show(tcp_ports, &tcp_num, udp_ports, &udp_num);
            break;
        case 6:
            G_STRING_UTILITY.draw_tcp_udp_ports_from_args(conf_args6, tcp_ports, &tcp_num, udp_ports, &udp_num);
            show(tcp_ports, &tcp_num, udp_ports, &udp_num);
            break;
        }
    }

    return 0;
}