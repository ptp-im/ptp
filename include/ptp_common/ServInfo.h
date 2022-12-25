#ifndef SERVINFO_H_
#define SERVINFO_H_

#include "ptp_common/util.h"
#include "ptp_common/imconn.h"

#define MAX_RECONNECT_CNT	32
#define MIN_RECONNECT_CNT	4

typedef struct {
	string		server_ip;
	uint16_t	server_port;
	uint32_t	account_id;
    uint32_t	idle_cnt;
	uint32_t	reconnect_cnt;
    bool 	    reconnect_enable;
	CImConn*	serv_conn;
} serv_info_t;

template <class T>

void serv_init(serv_info_t* server_list, uint32_t server_count, uint32_t account_id)
{
	for (uint32_t i = 0; i < server_count; i++) {
		T* pConn = new T();
		pConn->Connect(server_list[i].server_ip.c_str(), server_list[i].server_port, i,account_id);
		server_list[i].serv_conn = pConn;
		server_list[i].idle_cnt = 0;
        server_list[i].account_id = account_id;
		server_list[i].reconnect_cnt = MIN_RECONNECT_CNT / 2;
	}
}

template <class T>
void serv_check_reconnect(serv_info_t* server_list, uint32_t server_count)
{
	T* pConn;
	for (uint32_t i = 0; i < server_count; i++) {
		pConn = (T*)server_list[i].serv_conn;
		if (!pConn) {
			server_list[i].idle_cnt++;
			if (server_list[i].idle_cnt >= server_list[i].reconnect_cnt && server_list[i].reconnect_enable) {
                pConn = new T();
				pConn->Connect(server_list[i].server_ip.c_str(), server_list[i].server_port, i,server_list[i].account_id);
				server_list[i].serv_conn = pConn;
			}
		}else{

        }
	}
}

template <class T>
void serv_reset(serv_info_t* server_list, uint32_t server_count, uint32_t serv_idx)
{
	if (serv_idx >= server_count) {
		return;
	}

	server_list[serv_idx].serv_conn = NULL;
	server_list[serv_idx].idle_cnt = 0;
    if(server_list[serv_idx].reconnect_cnt == 0){
        server_list[serv_idx].reconnect_cnt = MAX_RECONNECT_CNT / 2;
    }else{
        server_list[serv_idx].reconnect_cnt *= 2;
    }

	if (server_list[serv_idx].reconnect_cnt > MAX_RECONNECT_CNT) {
		server_list[serv_idx].reconnect_cnt = MIN_RECONNECT_CNT;
	}
    Logger::d("reconnect_cnt:%d accountId:%d",server_list[serv_idx].reconnect_cnt,server_list[serv_idx].account_id);
}

serv_info_t* init_server_config(const char* server_ip,uint16_t server_port, uint32_t& server_count);
#endif
