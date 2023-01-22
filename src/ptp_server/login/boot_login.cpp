#include "LoginConn.h"
#include "ptp_global/ConfigFileReader.h"
#include "ptp_global/version.h"
#include "HttpConn.h"
#include "ipparser.h"

#include "FileConfig.h"
#include "ptp_global/version.h"
#include "ptp_global/ConfigFileReader.h"
#include "ptp_protobuf/PTP.Other.pb.h"

IpParser *pIpParser = NULL;
string strMsfsUrl;
string strDiscovery;//发现获取地址

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
    bool isDebug = false;

    for (int i = 0; i < argc; ++i) {
        if(strcmp(argv[i], "--debug") == 0){
            isDebug = true;
        }else if(strcmp(argv[i], "--debug") == 0){

        }
    }
    if(argc == 0 && argv == nullptr){
        isDebug = true;
    }
    string server_name = "ptp_server_login";
    slog_set_append(true,isDebug, true,LOG_PATH);

    DEBUG_I("Server Version: %s: %s",server_name.c_str(), VERSION);
    DEBUG_I("Server Run At: %s %s", __DATE__, __TIME__);
    DEBUG_I("%s max files can open: %d ", server_name.c_str(),getdtablesize());
    if(!init_server_config()){
        DEBUG_E("init_server_config failed");
        return -1;
    }

    signal(SIGPIPE, SIG_IGN);
    CConfigFileReader config_file(get_config_path().c_str());

    char *client_listen_ip = config_file.GetConfigName("LOGIN_ClientListenIP");
    char *str_client_port = config_file.GetConfigName("LOGIN_ClientPort");
    char *http_listen_ip = config_file.GetConfigName("LOGIN_HttpListenIP");
    char *str_http_port = config_file.GetConfigName("LOGIN_HttpPort");
    char *msg_server_listen_ip = config_file.GetConfigName("LOGIN_MSG_ListenIP");
    char *str_msg_server_port = config_file.GetConfigName("LOGIN_MSG_ListenPort");
    char *str_msfs_url = config_file.GetConfigName("URL_msfs");
    char *str_discovery = config_file.GetConfigName("URL_discovery");

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
    init_login_conn();
    DEBUG_I("%s client listen on: %s:%s",server_name.c_str(), client_listen_ip, str_client_port);
    init_http_conn();
    DEBUG_I("%s http listen on: http://%s:%s",server_name.c_str(), http_listen_ip, str_http_port);
    DEBUG_I("%s looping...",server_name.c_str());
    writePid(server_name);
    netlib_eventloop();
    return 0;
}
