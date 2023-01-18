#ifndef __MSG_SRV_CONN_H__
#define __MSG_SRV_CONN_H__

#include "ptp_global/ImConn.h"
#include "ptp_protobuf/ImPdu.h"
#include "FileConfig.h"
#include "CachePool.h"
#include "Request.h"

typedef struct {
	uint32_t msg_id;
	uint32_t from_id;
	uint64_t timestamp;
} msg_ack_t;

class CImUser;

class CMsgSrvConn : public CImConn
{
public:
	CMsgSrvConn();
	virtual ~CMsgSrvConn();
    uint32_t GetUserId() { return m_user_id; }
    void SetUserId(uint32_t user_id) { m_user_id = user_id; }
    net_handle_t GetHandle() { return m_handle; }
    uint16_t GetPduVersion() { return m_pdu_version; }
    uint32_t GetClientType() { return m_client_type; }
    void SetOpen() { m_bOpen = true; }
    bool IsOpen() { return m_bOpen; }
    void SetKickOff() { m_bKickOff = true; }
    bool IsKickOff() { return m_bKickOff; }
    uint32_t GetOnlineStatus() { return m_online_status; }

    void SetHandle(net_handle_t handle) { m_handle = handle; }

    void SetShareKey(string share_key) { m_share_key = share_key; }
    void SetIv(string iv) { m_iv = iv; }
    void SetAad(string aad) { m_aad = aad; }
    void SetPrvKey(string prv_key) { m_prv_key = prv_key; }
    void SetTest(bool test) { m_test = test; }

    void SetCaptcha(string captcha) { m_captcha = captcha; }
    void SetAddress(string address){ m_address_hex = address;}

    string GetAddress(){return m_address_hex;}
    string GetPrvKey(){return m_prv_key;}

    string GetCaptcha(){return m_captcha;}

    bool GetTest() { return m_test; }

	virtual void Close(bool kick_user = false);
	virtual void OnConnect(net_handle_t handle);
	virtual void OnClose();
	virtual inline void OnTimer(uint64_t curr_tick);
    int SendPdu(ImPdu* pPdu);
	virtual void HandlePdu(CImPdu* pPdu);
    void HandleNextResponse(ImPdu* pPdu);
    ImPdu * ReadTestPdu();

    uint32_t        m_client_type;        //客户端登录方式
    uint32_t        m_online_status;      //在线状态 1-online, 2-off-line, 3-leave
    string 			m_client_version;	// e.g MAC/2.2, or WIN/2.2
    uint32_t        m_login_time;
private:
    void _HandleHeartBeatNotify(CRequest *requestPdu);

private:
    uint32_t        m_user_id;
    bool			m_bOpen;	// only DB validate passed will be set to true;
    bool            m_bKickOff;
    uint64_t		m_connected_time;
    uint32_t		m_last_seq_no;
    uint16_t		m_pdu_version;
    list<msg_ack_t>	m_send_msg_list;
    uint32_t		m_msg_cnt_per_sec;

    string          m_share_key;
    string          m_iv;
    string          m_aad;
    string          m_prv_key;


    string          m_captcha;
    string          m_address_hex;

    bool            m_test;
    CSimpleBuffer   m_test_buf;

};

void init_msg_conn();
CMsgSrvConn *FindMsgSrvConnByHandle(uint32_t conn_handle);
void addMsgSrvConnByHandle(uint32_t conn_handle,CMsgSrvConn *pConn);
int run_ptp_server_msg(int argc, char* argv[]);

#endif /* __MSG_SRV_CONN_H__ */
