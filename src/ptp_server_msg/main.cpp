#include <cstdio>
#include <cassert>
#include "ptp_business/CachePool.h"
#include "ptp_global/ConfigFileReader.h"
#include "ptp_global/version.h"
#include "MsgSrvConn.h"

#define CONFIG_PATH             "../../conf/bd_server.conf"

void quit_route(){
    netlib_stop_event();
}

void ptp_server_msg_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
    if (msg == NETLIB_MSG_CONNECT)
    {
        auto* pConn = new CMsgSrvConn();
        pConn->OnConnect((int)handle);
    }
    else
    {
        DEBUG_I("!!!error msg: %d ", msg);
    }
}

int main(int argc, char* argv[])
{
    string server_name = "ptp_server_msg";
    if ((argc == 2) && (strcmp(argv[1], "-v") == 0)) {
        printf("Server Version: %s: %s\n",server_name.c_str(), VERSION);
        printf("Server Build: %s %s\n", __DATE__, __TIME__);
        return 0;
    }
    signal(SIGPIPE, SIG_IGN);
    srand(time(nullptr));
    DEBUG_I("%s max files can open: %d ", server_name.c_str(),getdtablesize());

    CacheManager::setConfigPath(CONFIG_PATH);
    CacheManager* pCacheManager = CacheManager::getInstance();
    if (!pCacheManager) {
        DEBUG_E("CacheManager init failed");
        return -1;
    }
    CConfigFileReader config_file(CONFIG_PATH);
    char* msg_listen_ip = config_file.GetConfigName("MSG_ListenIP");
    char* msg_listen_port_str = config_file.GetConfigName("MSG_ListenPort");
    if (!msg_listen_ip || !msg_listen_port_str) {
        DEBUG_E("config file miss ws_listen, exit... ");
        return -1;
    }
    uint16_t msg_listen_port = atoi(msg_listen_port_str);
    int ret = netlib_init();
    if (ret == NETLIB_ERROR)
        return ret;
    CStrExplode listen_ip_list(msg_listen_ip, ';');
    for (uint32_t i = 0; i < listen_ip_list.GetItemCnt(); i++) {
        ret = netlib_listen(listen_ip_list.GetItem(i), msg_listen_port, ptp_server_msg_callback, nullptr);
        if (ret == NETLIB_ERROR)
            return ret;
    }
    DEBUG_I("%s looping...",server_name.c_str());
    writePid(server_name+".pid");
    netlib_eventloop();
    return 0;
}
