#include "BusinessSrvConn.h"
#include "ProxyTask.h"
#include "HandlerMap.h"
#include "PTP.Other.pb.h"
#include "ptp_global/ThreadPool.h"
#include "ptp_global/ConfigFileReader.h"
#include "ptp_global/version.h"
#include "DBPool.h"
#include "CachePool.h"
#include "HttpClient.h"
#include "SyncCenter.h"
#include "define.h"
//
//#include "models/AudioModel.h"
//#include "models/MessageModel.h"
//#include "models/SessionModel.h"
//#include "models/RelationModel.h"
//#include "models/UserModel.h"
//#include "models/GroupModel.h"
//#include "models/GroupMessageModel.h"
//#include "models/FileModel.h"

static ConnMap_t g_proxy_conn_map;
static ConnMap_t g_uuid_conn_map;
static CHandlerMap* s_handler_map;

uint32_t CBusinessSrvConn::s_uuid_alloctor = 0;
CLock CBusinessSrvConn::s_list_lock;
list<ResponsePdu_t*> CBusinessSrvConn::s_response_pdu_list;
static CThreadPool g_thread_pool;

void proxy_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	uint64_t cur_time = get_tick_count();
	for (auto it = g_proxy_conn_map.begin(); it != g_proxy_conn_map.end(); ) {
		auto it_old = it;
		it++;

		auto* pConn = (CBusinessSrvConn*)it_old->second;
		pConn->OnTimer(cur_time);
	}
}


void proxy_loop_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	CBusinessSrvConn::SendResponsePduList();
}

/*
 * 用于优雅的关闭连接：
 * 服务器收到SIGTERM信号后，发送ImPduStopReceivePacket数据包给每个连接，
 * 通知消息服务器不要往自己发送数据包请求，
 * 然后注册4s后调用的回调函数，回调时再退出进程
 */
void exit_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	DEBUG_I("exit_callback...");
	exit(0);
}

static void sig_handler(int sig_no)
{
	if (sig_no == SIGTERM) {
		DEBUG_I("receive SIGTERM, prepare for exit");
        ImPdu cPdu;
//        IM::Server::IMStopReceivePacket msg;
//        msg.set_result(0);
//        cPdu.SetPBMsg(&msg);
//        cPdu.SetServiceId(IM::BaseDefine::SID_OTHER);
//        cPdu.SetCommandId(IM::BaseDefine::CID_OTHER_STOP_RECV_PACKET);
//        for (auto & it : g_proxy_conn_map) {
//            auto* pConn = (CBusinessSrvConn*)it.second;
//            pConn->SendPdu(&cPdu);
//        }
        // Add By ZhangYuanhao
        // Before stop we need to stop the sync thread,otherwise maybe will not sync the internal data any more
//        CSyncCenter::getInstance()->stopSync();
        
        // callback after 4 second to exit process;
		netlib_register_timer(exit_callback, NULL, 4000);
	}
}

int init_proxy_conn(uint32_t thread_num)
{
	s_handler_map = CHandlerMap::getInstance();
    s_handler_map->Init();
	g_thread_pool.Init(thread_num);

	netlib_add_loop(proxy_loop_callback, NULL);

	signal(SIGTERM, sig_handler);

	return netlib_register_timer(proxy_timer_callback, NULL, 1000);
}

CBusinessSrvConn* get_proxy_conn_by_uuid(uint32_t uuid)
{
	CBusinessSrvConn* pConn = NULL;
	auto it = g_uuid_conn_map.find(uuid);
	if (it != g_uuid_conn_map.end()) {
		pConn = (CBusinessSrvConn *)it->second;
	}

	return pConn;
}

CBusinessSrvConn::CBusinessSrvConn()
{
	m_uuid = ++CBusinessSrvConn::s_uuid_alloctor;
	if (m_uuid == 0) {
		m_uuid = ++CBusinessSrvConn::s_uuid_alloctor;
	}

	g_uuid_conn_map.insert(make_pair(m_uuid, this));
}

CBusinessSrvConn::~CBusinessSrvConn()
{

}

void CBusinessSrvConn::Close()
{
	if (m_handle != NETLIB_INVALID_HANDLE) {
		netlib_close(m_handle);
		g_proxy_conn_map.erase(m_handle);

		g_uuid_conn_map.erase(m_uuid);
	}

	ReleaseRef();
}

void CBusinessSrvConn::OnConnect(net_handle_t handle)
{
	m_handle = handle;

	g_proxy_conn_map.insert(make_pair(handle, this));

	netlib_option(handle, NETLIB_OPT_SET_CALLBACK, (void*)imconn_callback);
	netlib_option(handle, NETLIB_OPT_SET_CALLBACK_DATA, (void*)&g_proxy_conn_map);
	netlib_option(handle, NETLIB_OPT_GET_REMOTE_IP, (void*)&m_peer_ip);
	netlib_option(handle, NETLIB_OPT_GET_REMOTE_PORT, (void*)&m_peer_port);

	DEBUG_I("connect from %s:%d, handle=%d", m_peer_ip.c_str(), m_peer_port, m_handle);
}

// 由于数据包是在另一个线程处理的，所以不能在主线程delete数据包，所以需要Override这个方法
void CBusinessSrvConn::OnRead()
{
	for (;;) {
		uint32_t free_buf_len = m_in_buf.GetAllocSize() - m_in_buf.GetWriteOffset();
		if (free_buf_len < READ_BUF_SIZE)
			m_in_buf.Extend(READ_BUF_SIZE);

		int ret = netlib_recv(m_handle, m_in_buf.GetBuffer() + m_in_buf.GetWriteOffset(), READ_BUF_SIZE);
		if (ret <= 0)
			break;

		m_recv_bytes += ret;
		m_in_buf.IncWriteOffset(ret);
		m_last_recv_tick = get_tick_count();
	}

	uint32_t pdu_len = 0;
    try {
        while ( ImPdu::IsPduAvailable(m_in_buf.GetBuffer(), m_in_buf.GetWriteOffset(), pdu_len) ) {
            HandlePduBuf(m_in_buf.GetBuffer(), pdu_len);
            m_in_buf.Read(NULL, pdu_len);
        }
    } catch (CPduException& ex) {
        DEBUG_I("!!!catch exception, err_code=%u, err_msg=%s, close the connection ",
            ex.GetErrorCode(), ex.GetErrorMsg());
        OnClose();
    }
	
}


void CBusinessSrvConn::OnClose()
{
	Close();
}

void CBusinessSrvConn::OnTimer(uint64_t curr_tick)
{
	if (curr_tick > m_last_send_tick + SERVER_HEARTBEAT_INTERVAL) {
        
        ImPdu cPdu;
        PTP::Other::HeartBeatNotify msg;
        cPdu.SetPBMsg(&msg,CID_HeartBeatNotify,0);
		SendPdu(&cPdu);
	}

	if (curr_tick > m_last_recv_tick + SERVER_TIMEOUT) {
		DEBUG_I("proxy connection timeout %s:%d", m_peer_ip.c_str(), m_peer_port);
		Close();
	}
}

void CBusinessSrvConn::HandlePduBuf(uchar_t* pdu_buf, uint32_t pdu_len)
{
    ImPdu* pPdu = NULL;
    pPdu = (ImPdu*)ImPdu::ReadPdu(pdu_buf, pdu_len);
    if (pPdu->GetCommandId() == CID_HeartBeatNotify) {
        return;
    }
    
    pdu_handler_t handler = s_handler_map->GetHandler(pPdu->GetCommandId());
    DEBUG_I("CBusinessSrvConn::HandlePduBuf cid=%d,pid:%d", pPdu->GetCommandId(),getpid());
    if (handler) {
        CTask* pTask = new CProxyTask(m_uuid, handler, pPdu);
        g_thread_pool.AddTask(pTask);
    } else {
        DEBUG_I("no handler for cid=%d", pPdu->GetCommandId());
    }
}

/*
 * static method
 * add response pPdu to send list for another thread to send
 * if pPdu == NULL, it means you want to close connection with conn_uuid
 * e.g. parse packet failed
 */
void CBusinessSrvConn::AddResponsePdu(uint32_t conn_uuid, ImPdu* pPdu)
{
	auto* pResp = new ResponsePdu_t;
	pResp->conn_uuid = conn_uuid;
	pResp->pPdu = pPdu;

	s_list_lock.lock();
	s_response_pdu_list.push_back(pResp);
	s_list_lock.unlock();
}

void CBusinessSrvConn::SendResponsePduList()
{
	s_list_lock.lock();
	while (!s_response_pdu_list.empty()) {
		ResponsePdu_t* pResp = s_response_pdu_list.front();
		s_response_pdu_list.pop_front();
		s_list_lock.unlock();

		CBusinessSrvConn* pConn = get_proxy_conn_by_uuid(pResp->conn_uuid);
		if (pConn) {
			if (pResp->pPdu) {
				pConn->SendPdu(pResp->pPdu);
			} else {
				DEBUG_I("close connection uuid=%d by parse pdu error\b", pResp->conn_uuid);
				pConn->Close();
			}
		}

		if (pResp->pPdu)
			delete pResp->pPdu;

		delete pResp;

		s_list_lock.lock();
	}

	s_list_lock.unlock();
}


void proxy_serv_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
    if (msg == NETLIB_MSG_CONNECT)
    {
        CBusinessSrvConn* pConn = new CBusinessSrvConn();
        pConn->OnConnect(handle);
    }
    else
    {
        DEBUG_I("!!!error msg: %d", msg);
    }
}

int run_ptp_server_business(int argc, char* argv[])
{
    if ((argc == 2) && (strcmp(argv[1], "-v") == 0)) {
        printf("Server Version: DBProxyServer/%s\n", VERSION);
        printf("Server Build: %s %s\n", __DATE__, __TIME__);
        return 0;
    }

    signal(SIGPIPE, SIG_IGN);
    srand(time(NULL));

    CacheManager::setConfigPath(CONFIG_PATH);
    CacheManager* pCacheManager = CacheManager::getInstance();
    if (!pCacheManager) {
        DEBUG_I("CacheManager init failed");
        return -1;
    }
//    CDBManager::setConfigPath(CONFIG_PATH);
//    CDBManager* pDBManager = CDBManager::getInstance();
//    if (!pDBManager) {
//        DEBUG_I("DBManager init failed");
//        return -1;
//    }
    // 主线程初始化单例，不然在工作线程可能会出现多次初始化
//    if (!CAudioModel::getInstance()) {
//        return -1;
//    }
//
//    if (!CGroupMessageModel::getInstance()) {
//        return -1;
//    }
//
//    if (!CGroupModel::getInstance()) {
//        return -1;
//    }
//
//    if (!CMessageModel::getInstance()) {
//        return -1;
//    }
//
//    if (!CSessionModel::getInstance()) {
//        return -1;
//    }
//
//    if(!CRelationModel::getInstance())
//    {
//        return -1;
//    }
//
//    if (!CUserModel::getInstance()) {
//        return -1;
//    }
//
//    if (!CFileModel::getInstance()) {
//        return -1;
//    }
//

    CConfigFileReader config_file(CONFIG_PATH);

    char* listen_ip = config_file.GetConfigName("Business_ListenIP");
    char* str_listen_port = config_file.GetConfigName("Business_ListenPort");
    char* str_thread_num = config_file.GetConfigName("Business_ThreadNum");
    char* str_file_site = config_file.GetConfigName("MSFS_Site");

    if (!listen_ip || !str_listen_port || !str_thread_num || !str_file_site) {
        DEBUG_I("missing ListenIP/ListenPort/ThreadNum/MsfsSite, exit...");
        return -1;
    }

    uint16_t listen_port = atoi(str_listen_port);
    uint32_t thread_num = atoi(str_thread_num);

    string strFileSite(str_file_site);

    int ret = netlib_init();

    if (ret == NETLIB_ERROR)
        return ret;

    /// yunfan add 2014.9.28
    // for 603 push
    curl_global_init(CURL_GLOBAL_ALL);
    /// yunfan add end

    init_proxy_conn(thread_num);
//    CSyncCenter::getInstance()->init();
//    CSyncCenter::getInstance()->startSync();

    CStrExplode listen_ip_list(listen_ip, ';');
    for (uint32_t i = 0; i < listen_ip_list.GetItemCnt(); i++) {
        ret = netlib_listen(listen_ip_list.GetItem(i), listen_port, proxy_serv_callback, NULL);
        if (ret == NETLIB_ERROR)
            return ret;
    }

    DEBUG_I("server start listen on: %s:%d", listen_ip,  listen_port);
    DEBUG_I("ptp_server_business looping...,pid=%d",getpid());
    writePid();
    return 0;
}

