#include "MsgSrvConn.h"
#include "ptp_global/global_define.h"
#include "ptp_global/Helpers.h"
#include "ptp_crypto/crypto_helpers.h"
#include "ptp_crypto/secp256k1_helpers.h"
#include "ptp_global/AttachData.h"
#include "ptp_wallet/HDKey.h"
#include "PTP.Auth.pb.h"
#include "PTP.Server.pb.h"
#include "ImUser.h"
#include "HandlerMap.h"

using namespace PTP::Common;

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
    CRequest request;
    CResponse response;
    request.SetPdu((ImPdu*)pPdu);
    if(pPdu->GetCommandId() == CID_HeartBeatNotify){
        _HandleHeartBeatNotify(&request,&response);
        return;
    }
    DEBUG_D("HandlePdu,cid=%d",pPdu->GetCommandId());

    if(pPdu->GetCommandId() == CID_AuthCaptchaReq){
        _HandleAuthCaptchaRequest(&request,&response);
    }

    if(pPdu->GetCommandId() == CID_AuthLoginReq){
        _HandleAuthLoginRequest(&request,&response);
    }
    if (!IsOpen() && IsKickOff()) {
        DEBUG_I("HandlePdu, wrong msg. ");
        throw CPduException(pPdu->GetServiceId(), pPdu->GetCommandId(), ERROR_CODE_WRONG_SERVICE_ID, "HandlePdu error, user not login. ");
        return;
    }
    if(response.isPduValid()){
        if(response.isNext()){
            HandleNextResponse(response.GetPdu());
        }else{
            SendPdu(response.GetPdu());
        }
    }
}

void CMsgSrvConn::HandleNextResponse(ImPdu* pPdu){
    auto cid = pPdu->GetCommandId();
    DEBUG_D("HandleBusinessResponse cid=%d",cid);
    if(s_handler_map == nullptr){
        s_handler_map = CHandlerMap::getInstance();
        s_handler_map->Init();
    }
    auto m_pdu_handler = s_handler_map->GetHandler(cid);
    if(m_pdu_handler != nullptr){
        CRequest request;
        CResponse response;
        request.SetPdu(pPdu);
        m_pdu_handler(&request, &response);
        if(response.isPduValid()){
            if(response.isNext()){
                HandleNextResponse(response.GetPdu());
            }else{
                SendPdu(response.GetPdu());
            }
        }
    }
}

void CMsgSrvConn::_HandleHeartBeatNotify(CRequest *requestPdu,CResponse *responsePdu)
{
    responsePdu->SetPdu(requestPdu->GetPdu());
}

void CMsgSrvConn::_HandleAuthCaptchaRequest(CRequest *requestPdu,CResponse *responsePdu){

    PTP::Auth::AuthCaptchaReq msg;
    PTP::Auth::AuthCaptchaRes response;
    ERR error = NO_ERROR;
    while (true){
        if(!msg.ParseFromArray(requestPdu->GetPdu()->GetBodyData(), (int)requestPdu->GetPdu()->GetBodyLength()))
        {
            error = E_PB_PARSE_ERROR;
            break;
        }
        auto captcha = gen_random(6);
        SetCaptcha(captcha);
        response.set_captcha(captcha);
        unsigned char iv[16] = { 0 };
        gen_random_bytes(iv,16);
        unsigned char aad[16] = { 0 };
        gen_random_bytes(aad,16);
        string msg_data = format_sign_msg_data(captcha,SignMsgType_ptp);
        unsigned char signOut65[65];
        PTPWallet::MnemonicHelper::MnemonicResult mnemonic = PTPWallet::MnemonicHelper::generate();
        PTPWallet::HDKey hdKey = PTPWallet::HDKeyEncoder::makeEthRootKey(mnemonic.raw.data());
        PTPWallet::HDKeyEncoder::makeEthExtendedKey(hdKey, PTP_HD_PATH);
        string address = PTPWallet::HDKeyEncoder::getEthAddress(hdKey);
        string prvKeyStr = hdKey.privateKey.to_hex();
        secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
        ecdsa_sign_recoverable(ctx,msg_data,hdKey.privateKey.data(),signOut65);
        response.set_sign(signOut65,65);
        response.set_address(hex_to_string(address.substr(2)));
        response.set_iv(iv,16);
        response.set_aad(aad,16);
        SetPrvKey(prvKeyStr);
        SetIv(bytes_to_hex_string(iv,16));
        SetAad(bytes_to_hex_string(aad,16));
        DEBUG_D("server prvKey: %s",hdKey.privateKey.to_hex().c_str());
        DEBUG_D("server pubKey: %s",hdKey.publicKey.to_hex().c_str());
        DEBUG_D("server address: %s",address.c_str());
        DEBUG_D("msg_data: %s",msg_data.c_str());
        DEBUG_D("signOut65: %s", bytes_to_hex_string(signOut65,65).c_str());
        DEBUG_D("captcha: %s",response.captcha().c_str());
        DEBUG_D("iv: %s", bytes_to_hex_string(iv,16).c_str());
        DEBUG_D("aad: %s", bytes_to_hex_string(aad,16).c_str());
        break;
    }
    response.set_error(error);
    responsePdu->SendMsg(&response,
                         CID_AuthCaptchaRes,
                         requestPdu->GetPdu()->GetSeqNum());
}

void CMsgSrvConn::_HandleAuthLoginRequest(CRequest *request,CResponse *response){
    PTP::Auth::AuthLoginReq msg;
    PTP::Auth::AuthLoginRes msg_rsp;
    ERR error = NO_ERROR;
    while (true){
        if (GetAddress().length() != 0) {
            DEBUG_I("duplicate LoginRequest in the same conn ");
            error = E_LOGIN_ERROR;
            break;
        }

        if (GetCaptcha().length() == 0) {
            DEBUG_I("captcha is null ");
            error = E_LOGIN_ERROR;
            break;
        }

        if(!msg.ParseFromArray(request->GetPdu()->GetBodyData(), (int)request->GetPdu()->GetBodyLength()))
        {
            error = E_PB_PARSE_ERROR;
            break;
        }
        string captcha = msg.captcha();

        if (captcha != GetCaptcha()) {
            DEBUG_I("invalid captcha!");
            error = E_LOGIN_ERROR;
            break;
        }
        SetCaptcha("");

        const string& address = msg.address();
        SetAddress(address);

        m_client_version = msg.client_version();
        m_client_type = msg.client_type();
        m_online_status = USER_STAT_ONLINE;

        DEBUG_D("address=%s", address.c_str());
        DEBUG_D("client_type=%u ", msg.client_type());
        DEBUG_D("client_version=%s ",  msg.client_version().c_str());
        m_login_time = time(nullptr);

        CImUser *pImUser = CImUserManager::GetInstance()->GetImUserByAddress(GetAddress());
        if (!pImUser) {
            pImUser = new CImUser(GetAddress());
            CImUserManager::GetInstance()->AddImUserByAddress(GetAddress(), pImUser);
        }
        pImUser->AddMsgConn(GetHandle(),this);

        CDbAttachData attach(ATTACH_TYPE_HANDLE, GetHandle(), 0);

        PTP::Server::ServerLoginReq msg_next;
        msg_next.set_address(msg.address());
        msg_next.set_captcha(msg.captcha());
        msg_next.set_sign(msg.sign());
        msg_next.set_attach_data(attach.GetBuffer(), attach.GetLength());
        response->Next(&msg_next,CID_ServerLoginReq,request->GetPdu()->GetSeqNum());
        break;
    }

    if(error!= NO_ERROR){
        msg_rsp.set_error(error);
        response->SendMsg(&msg_rsp,CID_AuthLoginRes,request->GetPdu()->GetSeqNum());
    }
}

void CMsgSrvConn::_HandleAuthLoginResponse(CRequest *requestPdu,CResponse *responsePdu){

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
