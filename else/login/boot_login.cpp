#include "LoginConn.h"
#include "ptp_global/ConfigFileReader.h"
#include "ptp_global/version.h"
#include "HttpConn.h"
#include "ipparser.h"
#include "../boot.h"

IpParser *pIpParser = NULL;
string strMsfsUrl;
string strDiscovery;//发现获取地址

void quit_login(){
    netlib_stop_event();
}

static void client_callback(void *callback_data, uint8_t msg, uint32_t handle, void *pParam) {
    if (msg == NETLIB_MSG_CONNECT) {
        CLoginConn *pConn = new CLoginConn();
        pConn->OnConnect2(handle, LOGIN_CONN_TYPE_CLIENT);
    } else {
        DEBUG_E("!!!error msg: %d ", msg);
    }
}

// this callback will be replaced by imconn_callback() in OnConnect()
static void msg_serv_callback(void *callback_data, uint8_t msg, uint32_t handle, void *pParam) {
    DEBUG_I("msg_server come in");

    if (msg == NETLIB_MSG_CONNECT) {
        CLoginConn *pConn = new CLoginConn();
        pConn->OnConnect2(handle, LOGIN_CONN_TYPE_MSG_SERV);
    } else {
        DEBUG_I("!!!error msg: %d ", msg);
    }
}


static void http_callback(void *callback_data, uint8_t msg, uint32_t handle, void *pParam) {
    if (msg == NETLIB_MSG_CONNECT) {
        CHttpConn *pConn = new CHttpConn();
        pConn->OnConnect(handle);
    } else {
        DEBUG_I("!!!error msg: %d ", msg);
    }
}

int boot_login(int argc, char *argv[]) {
    if ((argc == 2) && (strcmp(argv[1], "-v") == 0)) {
        printf("Server Version: LoginServer/%s\n", VERSION);
        printf("Server Build: %s %s\n", __DATE__, __TIME__);
        return 0;
    }
    signal(SIGPIPE, SIG_IGN);
    CConfigFileReader config_file(CONFIG_PATH_LOGIN);
    char *client_listen_ip = config_file.GetConfigName("ClientListenIP");
    char *str_client_port = config_file.GetConfigName("ClientPort");
    char *http_listen_ip = config_file.GetConfigName("HttpListenIP");
    char *str_http_port = config_file.GetConfigName("HttpPort");
    char *msg_server_listen_ip = config_file.GetConfigName("MsgServerListenIP");
    char *str_msg_server_port = config_file.GetConfigName("MsgServerPort");
    char *str_msfs_url = config_file.GetConfigName("msfs");
    char *str_discovery = config_file.GetConfigName("discovery");

    if (!msg_server_listen_ip || !str_msg_server_port || !http_listen_ip
        || !str_http_port || !str_msfs_url || !str_discovery) {
        DEBUG_I("config item missing, exit... ");
        return -1;
    }

    uint16_t client_port = atoi(str_client_port);
    uint16_t msg_server_port = atoi(str_msg_server_port);
    uint16_t http_port = atoi(str_http_port);
    strMsfsUrl = str_msfs_url;
    strDiscovery = str_discovery;

    pIpParser = new IpParser();

    int ret = netlib_init();

    if (ret == NETLIB_ERROR)
        return ret;
    CStrExplode client_listen_ip_list(client_listen_ip, ';');
    for (uint32_t i = 0; i < client_listen_ip_list.GetItemCnt(); i++) {
        ret = netlib_listen(client_listen_ip_list.GetItem(i), client_port, client_callback, NULL);
        if (ret == NETLIB_ERROR)
            return ret;
    }
    DEBUG_I("start client ok!");
    CStrExplode msg_server_listen_ip_list(msg_server_listen_ip, ';');
    for (uint32_t i = 0; i < msg_server_listen_ip_list.GetItemCnt(); i++) {
        ret = netlib_listen(msg_server_listen_ip_list.GetItem(i), msg_server_port, msg_serv_callback, NULL);
        if (ret == NETLIB_ERROR)
            return ret;
    }
    DEBUG_I("start msg ok!");
    CStrExplode http_listen_ip_list(http_listen_ip, ';');
    for (uint32_t i = 0; i < http_listen_ip_list.GetItemCnt(); i++) {
        ret = netlib_listen(http_listen_ip_list.GetItem(i), http_port, http_callback, NULL);
        if (ret == NETLIB_ERROR)
            return ret;
    }
    DEBUG_I("start http ok!");
    init_login_conn();
    init_http_conn();
    DEBUG_I("now enter the event loop...");
    writePid("ptp_server_login.pid");
    netlib_eventloop();
    return 0;
}
