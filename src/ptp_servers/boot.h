#ifndef PTP_BOOT_H
#define PTP_BOOT_H
#include <iostream>

#define CONFIG_PATH             "../../conf/bd_server.conf"
#define CONFIG_PATH_MSFS        "../../conf/bd_msfs_server.conf"
#define CONFIG_PATH_PUSH        "../../conf/bd_push_server.conf"
#define CONFIG_PATH_LOGIN       "../../conf/bd_login_server.conf"

int get_msfs_config(
        char *listen_ip,
        uint16_t &listen_port,
        char *base_dir,
        int &filesPerDir,
        int &nPostThreadCount,
        int &nGetThreadCount);

int get_route_config(
        char *listen_ip,
        uint16_t &listen_port);

#endif //PTP_BOOT_H
