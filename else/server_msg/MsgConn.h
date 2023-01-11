#ifndef MSGCONN_H_
#define MSGCONN_H_

#include "ptp_global/ImConn.h"

typedef struct {
	uint32_t msg_id;
	uint32_t from_id;
	uint64_t timestamp;
} msg_ack_t;

class CImUser;

class CMsgConn : public CImConn
{
public:
	CMsgConn();
	virtual ~CMsgConn();
    uint32_t GetUserId() { return m_user_id; }
    void SetUserId(uint32_t user_id) { m_user_id = user_id; }
    uint32_t GetHandle() { return m_handle; }
    uint16_t GetPduVersion() { return m_pdu_version; }
    uint32_t GetClientType() { return m_client_type; }
    void SetOpen() { m_bOpen = true; }
    void SetCaptcha(string captcha) { m_captcha = captcha; }
    bool IsOpen() { return m_bOpen; }
    void SetKickOff() { m_bKickOff = true; }
    bool IsKickOff() { return m_bKickOff; }
    uint32_t GetOnlineStatus() { return m_online_status; }

    void SetShareKey(string share_key) { m_share_key = share_key; }
    void SetIv(string iv) { m_iv = iv; }
    void SetAad(string aad) { m_aad = aad; }
    void SetPrvKey(string prv_key) { m_prv_key = prv_key; }

	virtual void Close(bool kick_user = false);
	virtual void OnConnect(net_handle_t handle);
	virtual void OnClose();
	virtual inline void OnTimer(uint64_t curr_tick);
	virtual void HandlePdu(CImPdu* pPdu);
private:
    void _HandleHeartBeatNotify(CImPdu* pPdu);
    void _HandleAuthCaptchaRequest(CImPdu* pPdu);
    void _HandleAuthLoginRequest(CImPdu* pPdu);
    void _HandleAuthLoginResponse(CImPdu* pPdu,uint32_t msgConnHandle);
private:
    string          m_login_name;
    string          m_captcha;
    string          m_pub_key_64;
    uint32_t        m_user_id;
    bool			m_bOpen;	// only DB validate passed will be set to true;
    bool            m_bKickOff;
    uint64_t		m_login_time;
    uint32_t		m_last_seq_no;
    uint16_t		m_pdu_version;
    string 			m_client_version;	// e.g MAC/2.2, or WIN/2.2
    list<msg_ack_t>	m_send_msg_list;
    uint32_t		m_msg_cnt_per_sec;
    uint32_t        m_client_type;        //客户端登录方式
    uint32_t        m_online_status;      //在线状态 1-online, 2-off-line, 3-leave

    string          m_share_key;
    string          m_iv;
    string          m_aad;
    string          m_prv_key;

};

void init_msg_conn();

#endif /* MSGCONN_H_ */
