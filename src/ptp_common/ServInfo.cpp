#include "ptp_common/ServInfo.h"

serv_info_t* init_server_config(const char* server_ip,uint16_t server_port, uint32_t& server_count)
{
	server_count = 0;
	serv_info_t* server_list = new serv_info_t [server_count];
	for (uint32_t i = 0; i < server_count; i++) {
		server_list[i].server_ip = server_ip;
		server_list[i].server_port = server_port;
	}
	return server_list;
}
