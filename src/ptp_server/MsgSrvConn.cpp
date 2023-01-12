#include "MsgSrvConn.h"
#include "ptp_global/global_define.h"
#include "ptp_global/Helpers.h"
#include "ptp_crypto/crypto_helpers.h"
#include "ptp_global/ConfigFileReader.h"
#include "ptp_global/version.h"
#include "CachePool.h"
#include "ImUser.h"
#include "HandlerMap.h"
#include "BusinessClientConn.h"
#include "PTP.Auth.pb.h"
#include "PTP.Other.pb.h"

using namespace PTP::Common;

#define DEFAULT_CONCURRENT_DB_CONN_CNT  2

#define TIMEOUT_WATI_LOGIN_RESPONSE		15000	// 15 seconds
#define TIMEOUT_WAITING_MSG_DATA_ACK	15000	// 15 seconds
#define LOG_MSG_STAT_INTERVAL			300000	// log message miss status in every 5 minutes;
#define MAX_MSG_CNT_PER_SECOND			20	// user can not send more than 20 msg in one second
static ConnMap_t g_msg_conn_map;

static uint64_t	g_last_stat_tick;	// 上次显示丢包率信息的时间
static uint32_t g_up_msg_total_cnt = 0;		// 上行消息包总数
static uint32_t g_up_msg_miss_cnt = 0;		// 上行消息包丢数
static uint32_t g_down_msg_total_cnt = 0;	// 下行消息包总数
static uint32_t g_down_msg_miss_cnt = 0;	// 下行消息丢包数

static bool g_log_msg_toggle = true;	// 是否把收到的MsgData写入Log的开关，通过kill -SIGUSR2 pid 打开/关闭

static CHandlerMap* s_handler_map;

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

void msg_conn_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	ConnMap_t::iterator it_old;
	CMsgSrvConn* pConn = NULL;
	uint64_t cur_time = get_tick_count();

	for (ConnMap_t::iterator it = g_msg_conn_map.begin(); it != g_msg_conn_map.end(); ) {
		it_old = it;
		it++;

		pConn = (CMsgSrvConn*)it_old->second;
		pConn->OnTimer(cur_time);
	}

	if (cur_time > g_last_stat_tick + LOG_MSG_STAT_INTERVAL) {
		g_last_stat_tick = cur_time;
		DEBUG_I("up_msg_cnt=%u, up_msg_miss_cnt=%u, down_msg_cnt=%u, down_msg_miss_cnt=%u ",
			g_up_msg_total_cnt, g_up_msg_miss_cnt, g_down_msg_total_cnt, g_down_msg_miss_cnt);
	}
}

static void signal_handler_usr1(int sig_no)
{
	if (sig_no == SIGUSR1) {
		DEBUG_I("receive SIGUSR1 ");
		g_up_msg_total_cnt = 0;
		g_up_msg_miss_cnt = 0;
		g_down_msg_total_cnt = 0;
		g_down_msg_miss_cnt = 0;
	}
}

static void signal_handler_usr2(int sig_no)
{
	if (sig_no == SIGUSR2) {
		DEBUG_I("receive SIGUSR2 ");
		g_log_msg_toggle = !g_log_msg_toggle;
	}
}

static void signal_handler_hup(int sig_no)
{
	if (sig_no == SIGHUP) {
		DEBUG_I("receive SIGHUP exit... ");
		exit(0);
	}
}

void addMsgSrvConnByHandle(uint32_t conn_handle,CMsgSrvConn *pConn){
    g_msg_conn_map.insert(make_pair(conn_handle, pConn));
}

CMsgSrvConn *FindMsgSrvConnByHandle(uint32_t conn_handle) {
    CMsgSrvConn *pConn = NULL;
    auto it = g_msg_conn_map.find(conn_handle);
    if (it != g_msg_conn_map.end()) {
        pConn = (CMsgSrvConn *)it->second;
    }
    return pConn;
}


void init_msg_conn()
{
	g_last_stat_tick = get_tick_count();
	signal(SIGUSR1, signal_handler_usr1);
	signal(SIGUSR2, signal_handler_usr2);
	signal(SIGHUP, signal_handler_hup);
	netlib_register_timer(msg_conn_timer_callback, NULL, 1000);
}

CMsgSrvConn::CMsgSrvConn()
{
    m_test = false;
    m_user_id = 0;
    m_bOpen = false;
    m_bKickOff = false;
    m_last_seq_no = 0;
    m_msg_cnt_per_sec = 0;
    m_send_msg_list.clear();
    m_online_status = PTP::Common::USER_STAT_ONLINE;
}

CMsgSrvConn::~CMsgSrvConn(){}

void CMsgSrvConn::Close(bool kick_user)
{
    if (m_handle != NETLIB_INVALID_HANDLE) {
        netlib_close(m_handle);
        g_msg_conn_map.erase(m_handle);
    }

    CImUser *pImUser = CImUserManager::GetInstance()->GetImUserByAddress(GetAddress());
    if (pImUser) {
        pImUser->DelMsgConn(GetHandle());
        if (pImUser->IsMsgConnEmpty()) {
            CImUserManager::GetInstance()->RemoveImUser(pImUser);
        }
    }

    ReleaseRef();
}

void CMsgSrvConn::OnConnect(net_handle_t handle)
{
	m_handle = handle;
	m_connected_time = get_tick_count();
    addMsgSrvConnByHandle(handle,this);
	netlib_option(handle, NETLIB_OPT_SET_CALLBACK, (void*)imconn_callback);
	netlib_option(handle, NETLIB_OPT_SET_CALLBACK_DATA, (void*)&g_msg_conn_map);
	netlib_option(handle, NETLIB_OPT_GET_REMOTE_IP, (void*)&m_peer_ip);
	netlib_option(handle, NETLIB_OPT_GET_REMOTE_PORT, (void*)&m_peer_port);
}

void CMsgSrvConn::OnClose()
{
    DEBUG_I("Warning: peer closed. ");
	Close();
}

void CMsgSrvConn::OnTimer(uint64_t curr_tick)
{
	m_msg_cnt_per_sec = 0;
    if (CHECK_CLIENT_TYPE_MOBILE(GetClientType()))
    {
        if (curr_tick > m_last_recv_tick + MOBILE_CLIENT_TIMEOUT) {
            DEBUG_I("mobile client timeout, handle=%d, uid=%u ", m_handle, GetUserId());
            Close();
            return;
        }
    }
    else
    {
        if (curr_tick > m_last_recv_tick + CLIENT_TIMEOUT) {
            DEBUG_I("client timeout, handle=%d, uid=%u ", m_handle, GetUserId());
            Close();
            return;
        }
    }

	if (!IsOpen()) {
		if (curr_tick > m_connected_time + TIMEOUT_WATI_LOGIN_RESPONSE) {
			DEBUG_I("login timeout, handle=%d, uid=%u ", m_handle, GetUserId());
			Close();
			return;
		}
	}

	list<msg_ack_t>::iterator it_old;
	for (auto it = m_send_msg_list.begin(); it != m_send_msg_list.end(); ) {
		msg_ack_t msg = *it;
		it_old = it;
		it++;
		if (curr_tick >= msg.timestamp + TIMEOUT_WAITING_MSG_DATA_ACK) {
			DEBUG_I("!!!a msg missed, msg_id=%u, %u->%u ", msg.msg_id, msg.from_id, GetUserId());
			g_down_msg_miss_cnt++;
			m_send_msg_list.erase(it_old);
		} else {
			break;
		}
	}
}

void CMsgSrvConn::HandlePdu(CImPdu* pPdu)
{
    HandleNextResponse((ImPdu*)pPdu);
}

void CMsgSrvConn::HandleNextResponse(ImPdu* pPdu){
    auto cid = pPdu->GetCommandId();
    DEBUG_D("HandleNextResponse cid=%d",cid);
    if(s_handler_map == nullptr){
        s_handler_map = CHandlerMap::getInstance();
        s_handler_map->Init();
    }
    auto m_pdu_handler = s_handler_map->GetHandler(cid);
    if(m_pdu_handler != nullptr){
        CRequest request;
        request.SetHandle(GetHandle());
        CResponse response;
        request.SetPdu(pPdu);
        auto pDbMsgConn = get_business_serv_conn_for_login();
        if(pDbMsgConn->isEnable() && cid == CID_AuthLoginReq){
            if(!pDbMsgConn->IsOpen()){
                ImPdu pduLoginErr;
                PTP::Auth::AuthLoginRes msg_login_res;
                msg_login_res.set_error(PTP::Common::E_REASON_NO_DB_SERVER);
                pduLoginErr.SetPBMsg(&msg_login_res,CID_AuthLoginRes,pPdu->GetSeqNum());
                SendPdu(&pduLoginErr);
                return;
            }
        }
        m_pdu_handler(&request, &response);
        if(response.isPduValid()){
            if(response.isNext()){
                pDbMsgConn = get_business_serv_conn();
                if(!pDbMsgConn->isEnable()){
                    HandleNextResponse(response.GetPdu());
                }else{
                    if(pDbMsgConn->IsOpen()){
                        response.GetPdu()->SetReversed(GetHandle());
                        pDbMsgConn->SendPdu(response.GetPdu());
                    }else{
                        DEBUG_E("pDbMsgConn business is not open");
                        ImPdu pduLoginErr;
                        PTP::Other::HeartBeatNotify msg_res;
                        pduLoginErr.SetPBMsg(&msg_res,response.GetPdu()->GetCommandId(),response.GetPdu()->GetSeqNum());
                        pduLoginErr.SetFlag(PTP::Common::E_REASON_NO_DB_SERVER);
                        SendPdu(&pduLoginErr);
                    }
                }
            }else{
                SendPdu(response.GetPdu());
            }
        }
    }else{
        SendPdu(pPdu);
    }
}

int CMsgSrvConn::SendPdu(ImPdu *pPdu) {
    if(!m_test){
        return CImConn::SendPdu(pPdu);
    }else{
        m_test_buf.Position(0);
        m_test_buf.Write(pPdu->GetBuffer(),pPdu->GetLength());
    }
    return 0;
}

ImPdu * CMsgSrvConn::ReadTestPdu() {
    ImPdu* pPdu = nullptr;
    if(m_test){
        pPdu = (ImPdu*)CImPdu::ReadPdu(m_test_buf.GetBuffer(), m_test_buf.GetWriteOffset());
    }
    return pPdu;
}

int run_ptp_server_msg(int argc, char* argv[])
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
    uint16_t MSG_ENABLE_BUSINESS_ASYNC = 0;
    char* MSG_ENABLE_BUSINESS_ASYNC_str = config_file.GetConfigName("MSG_ENABLE_BUSINESS_ASYNC");
    if (MSG_ENABLE_BUSINESS_ASYNC_str) {
        MSG_ENABLE_BUSINESS_ASYNC = atoi(MSG_ENABLE_BUSINESS_ASYNC_str);

        if(MSG_ENABLE_BUSINESS_ASYNC == 1){
            set_enable_business_async(true);
        }

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

    if(MSG_ENABLE_BUSINESS_ASYNC == 1){
        //business client
        uint32_t db_server_count = 0;
        serv_info_t* db_server_list = read_server_config(&config_file, "MSG_DBServerIP", "MSG_DBServerPort", db_server_count);

        // 到BusinessServer的开多个并发的连接
        uint32_t concurrent_db_conn_cnt = DEFAULT_CONCURRENT_DB_CONN_CNT;
        uint32_t db_server_count2 = db_server_count * DEFAULT_CONCURRENT_DB_CONN_CNT;
        char* concurrent_db_conn = config_file.GetConfigName("MSG_ConcurrentDBConnCnt");
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

        if (db_server_count > 0) {
            init_business_serv_conn(db_server_list2, db_server_count2, concurrent_db_conn_cnt);
        }
    }

    DEBUG_I("%s looping...,pid=%d",server_name.c_str(),getpid());
    writePid();
    return 0;
}

