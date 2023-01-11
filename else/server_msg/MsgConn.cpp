#include "MsgConn.h"
#include "ptp_global/global_define.h"
#include "ptp_global/Helpers.h"
#include "ptp_protobuf/ImPdu.h"
#include "ptp_crypto/crypto_helpers.h"
#include "ptp_crypto/secp256k1_helpers.h"
#include "ptp_wallet/HDKey.h"
#include "PTP.Auth.pb.h"

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

void msg_conn_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	ConnMap_t::iterator it_old;
	CMsgConn* pConn = NULL;
	uint64_t cur_time = get_tick_count();

	for (ConnMap_t::iterator it = g_msg_conn_map.begin(); it != g_msg_conn_map.end(); ) {
		it_old = it;
		it++;

		pConn = (CMsgConn*)it_old->second;
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

void init_msg_conn()
{
	g_last_stat_tick = get_tick_count();
	signal(SIGUSR1, signal_handler_usr1);
	signal(SIGUSR2, signal_handler_usr2);
	signal(SIGHUP, signal_handler_hup);
	netlib_register_timer(msg_conn_timer_callback, NULL, 1000);
}


CMsgConn::CMsgConn()
{
    m_user_id = 0;
    m_bOpen = false;
    m_bKickOff = false;
    m_last_seq_no = 0;
    m_msg_cnt_per_sec = 0;
    m_send_msg_list.clear();
    m_online_status = PTP::Common::USER_STAT_ONLINE;
}

CMsgConn::~CMsgConn(){}

void CMsgConn::Close(bool kick_user)
{
    ReleaseRef();
}

void CMsgConn::OnConnect(net_handle_t handle)
{
	m_handle = handle;
	m_login_time = get_tick_count();
	g_msg_conn_map.insert(make_pair(handle, this));
	netlib_option(handle, NETLIB_OPT_SET_CALLBACK, (void*)imconn_callback);
	netlib_option(handle, NETLIB_OPT_SET_CALLBACK_DATA, (void*)&g_msg_conn_map);
	netlib_option(handle, NETLIB_OPT_GET_REMOTE_IP, (void*)&m_peer_ip);
	netlib_option(handle, NETLIB_OPT_GET_REMOTE_PORT, (void*)&m_peer_port);
}

void CMsgConn::OnClose()
{
    DEBUG_I("Warning: peer closed. ");
	Close();
}

void CMsgConn::OnTimer(uint64_t curr_tick)
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
		if (curr_tick > m_login_time + TIMEOUT_WATI_LOGIN_RESPONSE) {
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

void CMsgConn::HandlePdu(CImPdu* pPdu)
{
    if(pPdu->GetCommandId() == CID_HeartBeatNotify){
        _HandleHeartBeatNotify(pPdu);
        return;
    }

    DEBUG_D("HandlePdu,cid=%d",pPdu->GetCommandId());
    if (!IsOpen() && IsKickOff()) {
        DEBUG_I("HandlePdu, wrong msg. ");
        throw CPduException(pPdu->GetServiceId(), pPdu->GetCommandId(), ERROR_CODE_WRONG_SERVICE_ID, "HandlePdu error, user not login. ");
        return;
    }
}


void CMsgConn::_HandleHeartBeatNotify(CImPdu *pPdu)
{
    SendPdu(pPdu);
}

void CMsgConn::_HandleAuthCaptchaRequest(CImPdu* pPdu){
    PTP::Auth::AuthCaptchaReq msg;
    PTP::Auth::AuthCaptchaRes msg_rsp;
    ERR error = NO_ERROR;
    while (true){
        if(!msg.ParseFromArray(pPdu->GetBodyData(), (int)pPdu->GetBodyLength()))
        {
            error = E_PB_PARSE_ERROR;
            break;
        }
        auto captcha = gen_random(6);
        SetCaptcha(captcha);
        msg_rsp.set_captcha(captcha);
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
        msg_rsp.set_sign(signOut65,65);
        msg_rsp.set_address(hex_to_string(address.substr(2)));
        msg_rsp.set_iv(iv,16);
        msg_rsp.set_aad(aad,16);
        SetPrvKey(prvKeyStr);
        SetIv(bytes_to_hex_string(iv,16));
        SetAad(bytes_to_hex_string(aad,16));
        DEBUG_D("server pubKey: %s",hdKey.publicKey.to_hex().c_str());
        DEBUG_D("server address: %s",address.c_str());
        DEBUG_D("msg_data: %s",msg_data.c_str());
        DEBUG_D("signOut65: %s", bytes_to_hex_string(signOut65,65).c_str());
        DEBUG_D("captcha: %s",msg_rsp.captcha().c_str());
        DEBUG_D("iv: %s", bytes_to_hex_string(iv,16).c_str());
        DEBUG_D("aad: %s", bytes_to_hex_string(aad,16).c_str());
        break;
    }
    msg_rsp.set_error(error);
    ImPdu pdu;
    pdu.SetPBMsg(
            &msg_rsp,
            CID_AuthCaptchaRes,
            pPdu->GetSeqNum());

    SendPdu(&pdu);
}
void CMsgConn::_HandleAuthLoginRequest(CImPdu* pPdu){

}
void CMsgConn::_HandleAuthLoginResponse(CImPdu* pPdu,uint32_t msgConnHandle){

}

