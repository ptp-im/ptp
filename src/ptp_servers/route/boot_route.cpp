#include "RouteConn.h"
#include "ptp_global/NetLib.h"
#include "ptp_global/ConfigFileReader.h"
#include "ptp_global/Logger.h"
#include "ptp_global/Helpers.h"
#include "ptp_global/version.h"
#include "../boot.h"

void quit_route(){
    netlib_stop_event();
}

static void route_serv_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	if (msg == NETLIB_MSG_CONNECT)
	{
		CRouteConn* pConn = new CRouteConn();
		pConn->OnConnect(handle);
	}
	else
	{
		DEBUG_I("!!!error msg: %d ", msg);
	}
}

int boot_route(int argc, char* argv[])
{
	if ((argc == 2) && (strcmp(argv[1], "-v") == 0)) {
        DEBUG_I("Server Version: RouteServer/%s", VERSION);
        DEBUG_I("Server Build: %s %s", __DATE__, __TIME__);
		return 0;
	}

	signal(SIGPIPE, SIG_IGN);
	srand(time(NULL));

    char listen_ip[50];
    uint16_t listen_port;
    int ret = get_route_config(
            listen_ip,
            listen_port);

    if(ret == -1){
        return ret;
    }
	ret = netlib_init();

	if (ret == NETLIB_ERROR)
		return ret;

	CStrExplode listen_ip_list(listen_ip, ';');
	for (uint32_t i = 0; i < listen_ip_list.GetItemCnt(); i++) {
		ret = netlib_listen(listen_ip_list.GetItem(i), listen_port, route_serv_callback, NULL);
		if (ret == NETLIB_ERROR)
			return ret;
	}
	DEBUG_I("server start listen on: %s:%d", listen_ip,  listen_port);
	init_routeconn_timer_callback();
	DEBUG_I("now enter the event loop...");
    writePid("ptp_push_server.pid");
	netlib_eventloop();
	return 0;
}

