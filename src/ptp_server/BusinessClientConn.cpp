#include "BusinessClientConn.h"
#include "MsgSrvConn.h"
#include "HandlerMap.h"
#include "PTP.Common.pb.h"
#include "PTP.Other.pb.h"

using namespace PTP::Common;

static CHandlerMap* s_handler_map;

static bool m_enable_business_async = false;

static ConnMap_t g_business_server_conn_map;

static serv_info_t* g_db_server_list = NULL;
static uint32_t		g_db_server_count = 0;			// 到DBServer的总连接数
static uint32_t		g_db_server_login_count = 0;	// 到进行登录处理的DBServer的总连接数

static void business_server_conn_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	ConnMap_t::iterator it_old;
	CBusinessClientConn* pConn = NULL;
	uint64_t cur_time = get_tick_count();

	for (auto it = g_business_server_conn_map.begin(); it != g_business_server_conn_map.end(); ) {
		it_old = it;
		it++;

		pConn = (CBusinessClientConn*)it_old->second;
		if (pConn->IsOpen()) {
			pConn->OnTimer(cur_time);
		}
	}

	// reconnect DB Storage Server
	// will reconnect in 4s, 8s, 16s, 32s, 64s, 4s 8s ...
	serv_check_reconnect<CBusinessClientConn>(g_db_server_list, g_db_server_count);
}

void init_business_serv_conn(serv_info_t* server_list, uint32_t server_count, uint32_t concur_conn_cnt)
{
	g_db_server_list = server_list;
	g_db_server_count = server_count;

	uint32_t total_db_instance = server_count / concur_conn_cnt;
	g_db_server_login_count = (total_db_instance / 2) * concur_conn_cnt;
	DEBUG_I("DB server connection index for login business: [0, %u), for other business: [%u, %u) ",
			g_db_server_login_count, g_db_server_login_count, g_db_server_count);

	serv_init<CBusinessClientConn>(g_db_server_list, g_db_server_count);

	netlib_register_timer(business_server_conn_timer_callback, NULL, 1000);
}

// get a random db server connection in the range [start_pos, stop_pos)
static CBusinessClientConn* get_business_server_conn_in_range(uint32_t start_pos, uint32_t stop_pos)
{
	uint32_t i = 0;
	CBusinessClientConn* pDbConn = NULL;

	// determine if there is a valid DB server connection
	for (i = start_pos; i < stop_pos; i++) {
		pDbConn = (CBusinessClientConn*)g_db_server_list[i].serv_conn;
		if (pDbConn && pDbConn->IsOpen()) {
			break;
		}
	}

	// no valid DB server connection
	if (i == stop_pos) {
		return NULL;
	}

	// return a random valid DB server connection
	while (true) {
		int i = rand() % (stop_pos - start_pos) + start_pos;
		pDbConn = (CBusinessClientConn*)g_db_server_list[i].serv_conn;
		if (pDbConn && pDbConn->IsOpen()) {
			break;
		}
	}

	return pDbConn;
}

CBusinessClientConn* get_business_serv_conn_for_login()
{
	// 先获取login业务的实例，没有就去获取其他业务流程的实例
	CBusinessClientConn* pDBConn = get_business_server_conn_in_range(0, g_db_server_login_count);
	if (!pDBConn) {
		pDBConn = get_business_server_conn_in_range(g_db_server_login_count, g_db_server_count);
	}

    if(pDBConn == NULL){
        pDBConn = new CBusinessClientConn();
    }
	return pDBConn;
}

CBusinessClientConn* get_business_serv_conn()
{
	// 先获取其他业务流程的实例，没有就去获取login业务的实例
	CBusinessClientConn* pDBConn = get_business_server_conn_in_range(g_db_server_login_count, g_db_server_count);
	if (!pDBConn) {
		pDBConn = get_business_server_conn_in_range(0, g_db_server_login_count);
	}

    if(pDBConn == NULL){
        pDBConn = new CBusinessClientConn();
    }
	return pDBConn;
}

CBusinessClientConn::CBusinessClientConn()
{
	m_bOpen = false;
    s_handler_map = CHandlerMap::getInstance();
    s_handler_map->Init();
}

CBusinessClientConn::~CBusinessClientConn()
{

}

void CBusinessClientConn::Connect(const char* server_ip, uint16_t server_port, uint32_t serv_idx)
{
	DEBUG_I("Connecting to DB Storage Server %s:%d ", server_ip, server_port);

	m_serv_idx = serv_idx;
	m_handle = netlib_connect(server_ip, server_port, imconn_callback, (void*)&g_business_server_conn_map);

	if (m_handle != NETLIB_INVALID_HANDLE) {
        g_business_server_conn_map.insert(make_pair(m_handle, this));
	}
}

void CBusinessClientConn::Close()
{
	// reset server information for the next connect
	serv_reset<CBusinessClientConn>(g_db_server_list, g_db_server_count, m_serv_idx);

	if (m_handle != NETLIB_INVALID_HANDLE) {
		netlib_close(m_handle);
		g_business_server_conn_map.erase(m_handle);
	}

	ReleaseRef();
}

void CBusinessClientConn::OnConfirm()
{
	DEBUG_I("connect to db server success");
	m_bOpen = true;
	g_db_server_list[m_serv_idx].reconnect_cnt = MIN_RECONNECT_CNT / 2;
}

void CBusinessClientConn::OnClose()
{
	DEBUG_I("onclose from db server handle=%d", m_handle);
	Close();
}

void CBusinessClientConn::OnTimer(uint64_t curr_tick)
{
	if (curr_tick > m_last_send_tick + SERVER_HEARTBEAT_INTERVAL) {
        PTP::Other::HeartBeatNotify msg;
        ImPdu pdu;
        pdu.SetPBMsg(&msg);
        pdu.SetServiceId(0);
        pdu.SetCommandId(CID_HeartBeatNotify);
		SendPdu(&pdu);
	}

	if (curr_tick > m_last_recv_tick + SERVER_TIMEOUT) {
		DEBUG_I("conn to db server timeout");
		Close();
	}
}

void CBusinessClientConn::HandlePdu(CImPdu* pPdu)
{
    auto handler = s_handler_map->GetHandler(pPdu->GetCommandId());
    if (handler) {
        uint32_t handle  = pPdu->GetReversed();
        auto *pMsgSrvConn = (CMsgSrvConn*)FindMsgSrvConnByHandle(handle);
        if(pMsgSrvConn != nullptr){
            DEBUG_I("CBusinessClientConn::HandlePdu cid=%d,pid:%d", pPdu->GetCommandId(),getpid());
            pMsgSrvConn->HandleNextResponse((ImPdu *)pPdu);
        }else{
            if(pPdu->GetCommandId() != CID_HeartBeatNotify){
                DEBUG_E("CBusinessClientConn handler pMsgSrvConn is null for packet type: %d", pPdu->GetCommandId());
            }
        }
    } else {
        if(pPdu->GetCommandId() != CID_HeartBeatNotify){
            DEBUG_E("CBusinessClientConn no handler for packet type: %d", pPdu->GetCommandId());
        }
    }
}

bool CBusinessClientConn::isEnable(){
    return m_enable_business_async;
}

void set_enable_business_async(bool flag){
    m_enable_business_async = flag;
}
