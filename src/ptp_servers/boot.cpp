//
// Created by jack on 2023/1/9.
//

#include "boot.h"
#include "ptp_global/Logger.h"
#include "ptp_global/Helpers.h"
#include "ptp_global/ConfigFileReader.h"

int get_msfs_config(
        char *listen_ip,
        uint16_t &listen_port,
        char *base_dir,
        int &filesPerDir,
        int &nPostThreadCount,
        int &nGetThreadCount){
    DEBUG_I("MSFS Server max files can open: %d", getdtablesize());
    CConfigFileReader config_file(CONFIG_PATH);
    char *listen_ip_buf = config_file.GetConfigName("MSFS_ListenIP");
    char *str_listen_port = config_file.GetConfigName("MSFS_ListenPort");
    char *base_dir_buf = config_file.GetConfigName("MSFS_BaseDir");
    char *str_files_per_dir = config_file.GetConfigName("MSFS_FilesPerDir");
    char *str_post_thread_count = config_file.GetConfigName("MSFS_PostThreadCount");
    char *str_get_thread_count = config_file.GetConfigName("MSFS_GetThreadCount");

    if (!listen_ip_buf || !str_listen_port || !base_dir_buf || !str_files_per_dir || !str_post_thread_count ||
        !str_get_thread_count) {
        DEBUG_E("config file miss, exit...");
        return -1;
    }
    strcpy(listen_ip,listen_ip_buf);
    listen_ip[strlen(listen_ip_buf)] = '\0';
    strcpy(base_dir,base_dir_buf);
    base_dir[strlen(base_dir_buf)] = '\0';
    listen_port = atoi(str_listen_port);
    filesPerDir = atoi(str_files_per_dir);
    nPostThreadCount = atoi(str_post_thread_count);
    nGetThreadCount = atoi(str_get_thread_count);
    if (nPostThreadCount <= 0 || nGetThreadCount <= 0) {
        DEBUG_E("thread count is invalid");
        return -1;
    }
    DEBUG_I("MSFS server => %s:%s", listen_ip, str_listen_port);
    DEBUG_I("MSFS base_dir:%s", base_dir);
    DEBUG_I("MSFS filesPerDir:%d", filesPerDir);
    DEBUG_I("MSFS nPostThreadCount:%d", nPostThreadCount);
    DEBUG_I("MSFS nPostThreadCount:%d", nGetThreadCount);
    return 0;
}

int get_route_config(
        char *listen_ip,
        uint16_t &listen_port){

    CConfigFileReader config_file(CONFIG_PATH);
    char *listen_ip_buf = config_file.GetConfigName("ROUTE_ListenIP");
    char *str_listen_port = config_file.GetConfigName("ROUTE_ListenPort");

    if (!listen_ip_buf || !str_listen_port) {
        DEBUG_E("config file miss, exit...");
        return -1;
    }
    strcpy(listen_ip,listen_ip_buf);
    listen_ip[strlen(listen_ip_buf)] = '\0';

    DEBUG_I("ROUTE server => %s:%s", listen_ip, str_listen_port);
    return 0;
}