#include "ptp_global/NetLib.h"
#include "ptp_global/ConfigFileReader.h"
#include "ptp_global/version.h"
#include "HttpConn.h"
#import "MsgConn.h"
#include <cstdio>
#include <cassert>
#include "LoginServConn.h"
#include "FileServConn.h"
#include "CachePool.h"

#define DEFAULT_CONCURRENT_DB_CONN_CNT  2

void http_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	if (msg == NETLIB_MSG_CONNECT)
	{
		CHttpConn* pConn = new CHttpConn();
		pConn->OnConnect(handle);
	}
	else
	{
		DEBUG_E("!!!http api init error: %d ", msg);
	}
}

void websocket_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	if (msg == NETLIB_MSG_CONNECT)
	{
		CMsgConn* pConn = new CMsgConn();
		pConn->OnConnect(handle);
	}
	else
	{
		DEBUG_I("!!!websocket error msg: %d ", msg);
	}
}

int main(int argc, char* argv[])
{
    if ((argc == 2) && (strcmp(argv[1], "-v") == 0)) {
		printf("Server Version: BDWsServer/%s\n", VERSION);
		printf("Server Build: %s %s\n", __DATE__, __TIME__);
		return 0;
	}
    
	signal(SIGPIPE, SIG_IGN);
	srand(time(NULL));

	DEBUG_I("WsServer max files can open: %d ", getdtablesize());

//    CacheManager* pCacheManager = CacheManager::getInstance();
//    if (!pCacheManager) {
//        DEBUG_I("CacheManager init failed");
//        return -1;
//    }

    CConfigFileReader config_file("../bd_common/conf/bd_server.conf");

	char* http_listen_ip = config_file.GetConfigName("WS_httpListenIP");
	char* str_http_listen_port = config_file.GetConfigName("WS_httpListenPort");
    char* ip_addr1 = config_file.GetConfigName("WS_IpAddr1");	// 电信IP
    char* ip_addr2 = config_file.GetConfigName("WS_IpAddr2");	// 网通IP
    char* str_max_conn_cnt = config_file.GetConfigName("WS_MaxConnCnt");

	//websocket服务监听
	char* ws_listen_ip = config_file.GetConfigName("WS_ListenIP");
	char* str_ws_listen_port = config_file.GetConfigName("WS_ListenPort");

    // 没有IP2，就用第一个IP
    if (!ip_addr2) {
        ip_addr2 = ip_addr1;
    }

    uint16_t listen_port = atoi(str_ws_listen_port);
    uint32_t max_conn_cnt = atoi(str_max_conn_cnt);

    uint32_t db_server_count = 0;
	serv_info_t* db_server_list = read_server_config(&config_file, "WS_DBServerIP", "WS_DBServerPort", db_server_count);

    uint32_t login_server_count = 0;
    serv_info_t* login_server_list = read_server_config(&config_file, "LoginServerIP", "LoginServerPort", login_server_count);

    uint32_t file_server_count = 0;
    serv_info_t* file_server_list = read_server_config(&config_file, "FileServerIP",
                                                       "FileServerPort", file_server_count);

    uint32_t route_server_count = 0;
	serv_info_t* route_server_list = read_server_config(&config_file, "RouteServerIP", "RouteServerPort", route_server_count);

	// 到BusinessServer的开多个并发的连接
	uint32_t concurrent_db_conn_cnt = DEFAULT_CONCURRENT_DB_CONN_CNT;
	uint32_t db_server_count2 = db_server_count * DEFAULT_CONCURRENT_DB_CONN_CNT;
	char* concurrent_db_conn = config_file.GetConfigName("WS_ConcurrentDBConnCnt");
	if (concurrent_db_conn) {
		concurrent_db_conn_cnt  = atoi(concurrent_db_conn);
		db_server_count2 = db_server_count * concurrent_db_conn_cnt;
	}

	serv_info_t* db_server_list2 = NULL;
	if (db_server_count2 > 0) {
		db_server_list2 = new serv_info_t [ db_server_count2];
		for (uint32_t i = 0; i < db_server_count2; i++) {
			db_server_list2[i].server_ip = db_server_list[i / concurrent_db_conn_cnt].server_ip.c_str();
			db_server_list2[i].server_port = db_server_list[i / concurrent_db_conn_cnt].server_port;
		}
	}

	if (!ws_listen_ip || !str_ws_listen_port) {
		DEBUG_I("config file miss ws_listen, exit... ");
		return -1;
	}

	if (!http_listen_ip || !str_http_listen_port) {
		DEBUG_I("config file miss http_listen, exit... ");
		return -1;
	}

    uint16_t http_listen_port = atoi(str_http_listen_port);
    uint16_t ws_listen_port = atoi(str_ws_listen_port);

	int ret = netlib_init();
    
	if (ret == NETLIB_ERROR)
		return ret;
    
	CStrExplode listen_ip_list(http_listen_ip, ';');
	for (uint32_t i = 0; i < listen_ip_list.GetItemCnt(); i++) {
		ret = netlib_listen(listen_ip_list.GetItem(i), http_listen_port, http_callback, NULL);
		if (ret == NETLIB_ERROR)
			return ret;
	}

	init_http_conn();

    if (db_server_count > 0) {
		 init_db_serv_conn(db_server_list2, db_server_count2, concurrent_db_conn_cnt);
	}

	if (route_server_count > 0) {
//		 init_route_serv_conn(route_server_list, route_server_count);
	}

	//websocket服务开启
	CStrExplode ws_listen_ip_list(ws_listen_ip, ';');
	for (uint32_t i = 0; i < ws_listen_ip_list.GetItemCnt(); i++) {
		ret = netlib_listen(ws_listen_ip_list.GetItem(i), ws_listen_port, websocket_callback, NULL);
		if (ret == NETLIB_ERROR)
			return ret;
	}

	init_websocket_conn();

//    init_file_serv_conn(file_server_list, file_server_count);

//    init_login_serv_conn(login_server_list, login_server_count, ip_addr1, ip_addr2, listen_port, max_conn_cnt);

    DEBUG_I("http run at: http://%s:%d ",http_listen_ip,http_listen_port);
    DEBUG_I("ws run at: ws://%s:%d",ws_listen_ip,ws_listen_port);

    DEBUG_I("bd_ws_server websocket start ok!");
    
    writePid();
	netlib_eventloop();
	return 0;
}
