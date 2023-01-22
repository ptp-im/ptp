#include "MsgSrvConn.h"
#include "ptp_global/global_define.h"
#include "ptp_global/Helpers.h"
#include "ptp_crypto/crypto_helpers.h"
#include "ptp_global/ConfigFileReader.h"
#include "ptp_global/version.h"
#include "ptp_crypto/aes_encryption.h"
#include "CachePool.h"
#include "ImUser.h"
#include "actions/HandlerMap.h"
#include "BusinessClientConn.h"
#include "PTP.Auth.pb.h"
#include "PTP.Other.pb.h"
#include "FileConfig.h"

static bool g_is_websocket;

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
        DEBUG_I("g_is_websocket: %d ", g_is_websocket);
        auto* pConn = new CMsgSrvConn();
        pConn->m_is_websocket = g_is_websocket;
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
    DEBUG_D("addMsgSrvConnByHandle g_msg_conn_map size=%d",g_msg_conn_map.size());
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
    m_online_status = PTP::Common::USER_STAT_OFFLINE;

    ws_request = new Websocket_Request();
    ws_respond = new Websocket_Respond();
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

void CMsgSrvConn::OnRead()
{
    int i = 0;
    for (;;)
    {
        if(m_in_buf.GetWriteOffset() > MAX_READ_BUF_SIZE_MSG_SRV){
            OnClose();
            break;
        }
        uint32_t free_buf_len = m_in_buf.GetAllocSize() - m_in_buf.GetWriteOffset();
        if (free_buf_len < READ_BUF_SIZE_MSG_SRV)
            m_in_buf.Extend(READ_BUF_SIZE_MSG_SRV);
        int ret = netlib_recv(m_handle, m_in_buf.GetBuffer() + m_in_buf.GetWriteOffset(), READ_BUF_SIZE_MSG_SRV);
        //DEBUG_D("i#%d,pid=%d,handle=%d,ret=%d,offset=%d",i,getpid(),GetHandle(),ret,m_in_buf.GetWriteOffset());
        if (ret <= 0)
            break;

        m_recv_bytes += ret;
        m_in_buf.IncWriteOffset(ret);

        m_last_recv_tick = get_tick_count();
        i++;

    }
    //DEBUG_D("pid=%d,handle=%d,m_in_buf size=%d",getpid(),GetHandle(),m_in_buf.GetWriteOffset());
    CImPdu* pPdu = NULL;
    DEBUG_I("m_is_websocket: %d ", m_is_websocket);
    try
    {
        if(m_is_websocket){

            char *in_buf = (char *) m_in_buf.GetBuffer();
            uint32_t buf_len = m_in_buf.GetWriteOffset();
            if(buf_len != 16 && buf_len != 22){
                DEBUG_D("read buf_len: %d", buf_len);
            }
            if (!is_hand_shake) {
                strcpy(buff_, in_buf);
                char *b = strdup("Connection: Upgrade");
                if (strstr(in_buf, b) != NULL) {
                    char *c = strdup("Sec-WebSocket-Key:");
                    if (strstr(in_buf, c) != NULL) {
                        char request[1024] = {};
                        ws_respond->fetch_http_info(buff_);
                        ws_respond->parse_str(request);
                        //发送握手回复
                        Send(request, (uint32_t) strlen(request));
                        is_hand_shake = true;
                        memset(buff_, 0, sizeof(buff_));
                    } else {
                        Close();
                    }
                } else {
                    Close();
                }
                return;
            }
            ws_request->reset();
            ws_request->fetch_websocket_info(in_buf);

            if (in_buf) {
                memset(in_buf, 0, strlen(in_buf));
            }
            if (ws_request->opcode_ == 0x1) {
                Close();
            }if (ws_request->opcode_ == 0x2) {
                auto buf = (unsigned char *)ws_request->payload_;
                auto len = ws_request->payload_length_;
                while ( ( pPdu = CImPdu::ReadPdu(buf, len) ) )
                {
                    uint32_t pdu_len = pPdu->GetLength();
                    HandlePdu(pPdu);
                    m_in_buf.Read(NULL, pdu_len);
                    delete pPdu;
                    pPdu = NULL;
                }
            } else if (ws_request->opcode_ == 0x8) {//连接关闭
                Close();
            }
        }else{
            while ( ( pPdu = CImPdu::ReadPdu(m_in_buf.GetBuffer(), m_in_buf.GetWriteOffset()) ) )
            {
                uint32_t pdu_len = pPdu->GetLength();
                HandlePdu(pPdu);
                m_in_buf.Read(NULL, pdu_len);
                delete pPdu;
                pPdu = NULL;
            }
        }

    } catch (CPduException& ex) {
        DEBUG_E("!!!catch exception, sid=%u, cid=%u, err_code=%u, err_msg=%s, close the connection ",
                ex.GetServiceId(), ex.GetCommandId(), ex.GetErrorCode(), ex.GetErrorMsg());
        if (pPdu) {
            delete pPdu;
            pPdu = NULL;
        }
        OnClose();
    }
}
void CMsgSrvConn::HandlePdu(CImPdu* pPdu)
{
    HandleNextResponse((ImPdu*)pPdu,false);
}

void CMsgSrvConn::HandleNextResponse(ImPdu* pPdu,bool isNext){
    auto cid = (ActionCommands)pPdu->GetCommandId();

    if(pPdu->GetCommandId() != CID_AuthCaptchaReq
       && pPdu->GetCommandId() != CID_AuthLoginReq
       && pPdu->GetBodyLength() > 0
       && pPdu->GetReversed() == 1){

        auto cipherData = pPdu->GetBodyData();
        auto cipherDataLen = pPdu->GetBodyLength();

        unsigned char shared_secret[32];
        unsigned char iv[16];
        unsigned char aad[16];
        string shareKey = this->m_share_key;
        string ivHex = this->m_iv;
        string aadHex = this->m_aad;

        memcpy(shared_secret,hex_to_string(shareKey.substr(2)).data(),32);
        memcpy(iv,hex_to_string(ivHex.substr(2)).data(),16);
        memcpy(aad,hex_to_string(aadHex.substr(2)).data(),16);

        unsigned char decData[1024*1024];
        int decLen = aes_gcm_decrypt(
                cipherData, cipherDataLen,
                shared_secret, iv,
                aad,
                decData);

        if (decLen > 0) {
            pPdu->SetPBMsg(decData,decLen);
            pPdu->SetReversed(0);
            memset(decData, 0, sizeof(decData));
        }else{
            DEBUG_E("error decrypt body,cid:%d",pPdu->GetCommandId() );
            return;
        }
    }

    DEBUG_D("HandleNextResponse cid=%s", getActionCommandsName(cid).c_str());
    if(s_handler_map == nullptr){
        s_handler_map = CHandlerMap::getInstance();
        s_handler_map->Init();
    }
    auto m_pdu_handler = s_handler_map->GetHandler(cid);
    if(m_pdu_handler != nullptr){
        CRequest request;
        request.SetHandle(GetHandle());
        request.SetRequestPdu(pPdu);
        request.SetIsBusinessConn(isNext);
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
        m_pdu_handler(&request);
        if(request.HasNext()){
            if(request.GetNextResponsePdu() && request.IsNextResponsePduValid()){
                pDbMsgConn = get_business_serv_conn();
                if(!pDbMsgConn->isEnable()){
                    HandleNextResponse(request.GetNextResponsePdu(),true);
                }else{
                    if(pDbMsgConn->IsOpen()){
                        request.GetResponsePdu()->SetReversed(GetHandle());
                        pDbMsgConn->SendPdu(request.GetNextResponsePdu());
                    }else{
                        DEBUG_E("pDbMsgConn business is not open");
                        ImPdu pduLoginErr;
                        PTP::Other::HeartBeatNotify msg_res;
                        pduLoginErr.SetPBMsg(&msg_res,request.GetNextResponsePdu()->GetCommandId(),request.GetNextResponsePdu()->GetSeqNum());
                        pduLoginErr.SetFlag(PTP::Common::E_REASON_NO_DB_SERVER);
                        SendPdu(&pduLoginErr);
                    }
                }
            }
        }else{
            if(request.GetResponsePdu() && request.IsResponsePduValid()){
                SendPdu(request.GetResponsePdu());
            }
        }
    }else{
        Close();
    }
}

int CMsgSrvConn::SendPdu(ImPdu *pPdu) {
    if(!m_test){
        pPdu->Dump();
        if(pPdu->GetReversed() && !this->m_share_key.empty()){
            unsigned char shared_secret[32];
            unsigned char iv[16];
            unsigned char aad[16];
            string ivHex = this->m_iv;
            string aadHex =  this->m_aad;
            memcpy(shared_secret,hex_to_string(this->m_share_key.substr(2)).data(),32);
            memcpy(iv,hex_to_string(ivHex.substr(2)).data(),16);
            memcpy(aad,hex_to_string(aadHex.substr(2)).data(),16);

            unsigned char cipherData[1024*1024];
            int cipherDataLen = aes_gcm_encrypt(
                    pPdu->GetBodyData(),
                    (int)pPdu->GetBodyLength(),
                    shared_secret,
                    iv,aad,
                    cipherData);
            pPdu->SetPBMsg(cipherData,cipherDataLen);
            pPdu->Dump();
        }
        if(g_is_websocket){
            SendBufMessageToWS(pPdu->GetBuffer(),pPdu->GetLength());
            return 1;
        }else{
            return CImConn::SendPdu(pPdu);
        }

    }else{
        m_test_buf.Position(0);
        m_test_buf.Write(pPdu->GetBuffer(),pPdu->GetLength());
    }
    return 0;
}

void CMsgSrvConn::SendBufMessageToWS( void *data, int messageLength ) {
    std::string outFrame;
    if (messageLength > 20000) {
        return;
    }
    uint8_t payloadFieldExtraBytes = (messageLength <= 0x7d) ? 0 : 2;
    // header: 2字节, mask位设置为0(不加密), 则后面的masking key无须填写, 省略4字节
    uint8_t frameHeaderSize = 2 + payloadFieldExtraBytes;
    auto *frameHeader = new uint8_t[frameHeaderSize];
    memset(frameHeader, 0, frameHeaderSize);

//    fin位为1, 扩展位为0, 操作位为frameType
//    TEXT_FRAME=0x81,
//    BINARY_FRAME=0x82,
//    frameHeader[0] = static_cast<uint8_t>(0x80 | 0x1);
    frameHeader[0] = static_cast<uint8_t>(0x80 | 0x2);

    if (messageLength <= 0x7d) {
        frameHeader[1] = static_cast<uint8_t>(messageLength);
    } else {
        frameHeader[1] = 0x7e;
        uint16_t len = htons(messageLength);
        memcpy(&frameHeader[2], &len, payloadFieldExtraBytes);
    }

    // 填充数据
    uint32_t frameSize = frameHeaderSize + messageLength;
    char *frame = new char[frameSize];
    memcpy(frame, frameHeader, frameHeaderSize);
    memcpy(frame + frameHeaderSize, data, messageLength);
    //frame[frameSize] = '\0';
    Send(frame, frameSize);
    delete[] frame;
}

ImPdu * CMsgSrvConn::ReadTestPdu() {
    ImPdu* pPdu = nullptr;
    if(m_test){
        pPdu = (ImPdu*)CImPdu::ReadPdu(m_test_buf.GetBuffer(), m_test_buf.GetWriteOffset());
    }
    return pPdu;
}

int run_ptp_server_msg(int argc, char* argv[],bool isWs)
{
    g_is_websocket = isWs;
    bool isDebug = false;

    for (int i = 0; i < argc; ++i) {
        if(strcmp(argv[i], "--debug") == 0){
            isDebug = true;
        }
    }
    if(argc == 0 && argv == nullptr){
        isDebug = true;
    }
    string server_name = isWs ? "ptp_server_ws" : "ptp_server_msg";
    slog_set_append(true,isDebug, true,LOG_PATH);
    DEBUG_I("Server Version: %s: %s",server_name.c_str(), VERSION);
    DEBUG_I("Server Run At: %s %s", __DATE__, __TIME__);
    DEBUG_I("%s max files can open: %d ", server_name.c_str(),getdtablesize());
    DEBUG_I("g_is_websocket: %d ", g_is_websocket);
    signal(SIGPIPE, SIG_IGN);
    srand(time(nullptr));
    if(!init_server_config()){
        DEBUG_E("init_server_config failed");
        return -1;
    }

    CConfigFileReader config_file(get_config_path().c_str());
    char* msg_listen_ip = config_file.GetConfigName(isWs ? "WS_ListenIP":"MSG_ListenIP");
    char* msg_listen_port_str = config_file.GetConfigName(isWs ? "WS_ListenPort" : "MSG_ListenPort");
    if (!msg_listen_ip || !msg_listen_port_str) {
        DEBUG_E("config file miss ip or port, exit... ");
        return -1;
    }
    uint16_t MSG_ENABLE_BUSINESS_ASYNC = 0;
    char* MSG_ENABLE_BUSINESS_ASYNC_str = config_file.GetConfigName("MSG_ENABLE_BUSINESS_ASYNC");
    if (MSG_ENABLE_BUSINESS_ASYNC_str) {
        MSG_ENABLE_BUSINESS_ASYNC = string_to_int(MSG_ENABLE_BUSINESS_ASYNC_str);
        if(MSG_ENABLE_BUSINESS_ASYNC == 1){
            set_enable_business_async(true);
        }
    }
    if(MSG_ENABLE_BUSINESS_ASYNC == 0){
        CacheManager::setConfigPath(get_config_path());
        CacheManager* pCacheManager = CacheManager::getInstance();
        if (!pCacheManager) {
            DEBUG_E("CacheManager init failed");
            return -1;
        }
    }
    uint16_t msg_listen_port = string_to_int(msg_listen_port_str);
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
            concurrent_db_conn_cnt  = string_to_int(concurrent_db_conn);
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
    DEBUG_D("%s start listen on: %s:%d", server_name.c_str(),msg_listen_ip, msg_listen_port);
    DEBUG_I("%s looping...",server_name.c_str());
    writePid(server_name);
    return 0;
}

