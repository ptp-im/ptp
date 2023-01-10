#ifndef __MSG_CONN_CONN_H__
#define __MSG_CONN_CONN_H__


//#include <sstream>
//#include <cstring>
//#include <arpa/inet.h>
//#include <iostream>

#include "ptp_global/NetLib.h"
#include "ptp_global/Util.h"
#include "ptp_global/HttpParserWrapper.h"
#include "ptp_global/ImConn.h"
#include "ptp_global/ServInfo.h"
#include "ptp_global/global_define.h"
#include "HandlerMap.h"
#include "WebsocketRequest.h"
#include "WebsocketRespond.h"
#include "PTP.Common.pb.h"
#include "IM.BaseDefine.pb.h"
#include <unordered_map>

using namespace PTP::Common;

//using namespace IM::BaseDefine;

#define WEBSOCKET_CONN_TIMEOUT   60000

typedef struct {
    uint32_t msg_id;
    uint32_t from_id;
    uint64_t timestamp;
} msg_ack_t;

enum {
	WS_STATE_UNCONNECTED,
	WS_STATE_CONNECTED,
	WS_STATE_HANDSHARK,
	WS_STATE_AUTH,
	WS_STATE_CLOSED,
};

class CMsgConn : public CRefObject
{

public:
	CMsgConn();
	virtual ~CMsgConn();

    uint32_t GetOnlineStatus() { return m_online_status; }
    string GetCaptcha() { return m_captcha; }
    uint32_t GetLoginTime() { return m_login_time; }
    string GetPrvKey() { return m_prv_key; }
    string GetShareKey() { return m_share_key; }
    string GetIv() { return m_iv; }
    string GetAad() { return m_aad; }
    string GetAddressHex() { return m_address_hex; }
    string GetOsName() { return m_os_name; }
    string GetOsVersion() { return m_os_version; }
    string GetBrowserName() { return m_browser_name; }
    string GetBrowserVersion() { return m_browser_version; }
    string GetClientId() { return m_client_id; }
    bool GetIsIntel() { return m_is_intel; }
    uint32_t GetUserId() { return m_user_id; }
    uint32_t GetClientType() { return m_client_type; }
    string GetClientVersion() { return m_client_version; }
    uint32_t GetMsgCntPerSec() { return m_msg_cnt_per_sec; }
    uint32_t GetConnHandle() { return m_conn_handle; }
    uint32_t GetHandle() { return m_conn_handle; }
    uint32_t GetState() { return m_state; }
    bool IsOpen() { return m_bOpen; }
    bool IsKickOff() { return m_bKickOff; }
    char* GetPeerIP() { return (char*)m_peer_ip.c_str(); }

    void SetLoginTime(uint32_t t) { m_login_time = t; }
    void SetCaptcha(string captcha) { m_captcha = captcha; }
    void SetAddressHex(string address_hex) { m_address_hex = address_hex; }
    void SetShareKey(string share_key) { m_share_key = share_key; }
    void SetIv(string iv) { m_iv = iv; }
    void SetAad(string aad) { m_aad = aad; }
    void SetPrvKey(string prv_key) { m_prv_key = prv_key; }
	void SetUserId(uint32_t user_id) { m_user_id = user_id; }
    void SetState(uint32_t state) { m_state = state; }
    void SetClientInfo(string client_id,bool is_intel,string os_name,string os_version,string browser_name,string browser_version);
    void SetMsgCntPerSec() { m_msg_cnt_per_sec++; }
    void SetProperty( string client_version, uint32_t client_type,uint32_t online_status);
    void SetOpen() { m_bOpen = true; }
    void SetKickOff() { m_bKickOff = true; }
    void Set_testing_require_pdu() { m_testing_require_pdu = true; }
    CImPdu* Get_testing_require_pdu() { return m_sendPdu; }
    void SetConnHandle(uint32_t conn_handle) { m_conn_handle = conn_handle;}

    void updateUserStatus(uint32_t user_status);
    void SendUserStatusUpdate(uint32_t user_status);
    void HandlePduBuf(uchar_t* pdu_buf, uint32_t pdu_len);
    void SendPdu(CImPdu* pdu);
	int Send(void* data, int len);
    void Close();
    void OnConnect(net_handle_t handle);
    void OnRead();
    void OnWrite();
    void OnClose();
    void OnTimer(uint64_t curr_tick);
    virtual void OnWriteCompelete();
    void AddToSendList(uint32_t msg_id, uint32_t from_id);
    void DelFromSendList(uint32_t msg_id, uint32_t from_id);
	void SendMessageToWS(const char* msg);
    void toFrameDataPkt(const std::string &data);
    void SendBufMessageToWS(void *data,int messageLength);
    bool is_login_server_ok();
	char buff_[2048];
	int fd_;
	bool isHandshark;

protected:
    CImPdu *        m_sendPdu;
    uint32_t        m_online_status;
    string          m_client_version;
    uint32_t        m_client_type;
    bool            m_is_intel;
    uint32_t        m_login_time;
    string          m_os_name;
    string          m_os_version;
    string          m_browser_name;
    string          m_browser_version;
    string          m_client_id;
    string          m_captcha;
	uint32_t        m_user_id;
    string          m_address_hex;
    string          m_share_key;
    string          m_iv;
    string          m_aad;
    string          m_prv_key;
	uint32_t		m_conn_handle;

    bool			m_bOpen;	// only DB validate passed will be set to true;
    bool            m_bKickOff;

	bool			m_busy;
    bool            m_testing_require_pdu;
    bool            m_is_websocket;

    uint32_t        m_state;
	std::string		m_peer_ip;
	uint16_t		m_peer_port;
	CSimpleBuffer	m_out_buf;
    CSimpleBuffer   m_in_buf;

    uint32_t		m_msg_cnt_per_sec;
    uint64_t		m_last_send_tick;
	uint64_t		m_last_recv_tick;

    list<msg_ack_t>	m_send_msg_list;

    CHttpParserWrapper m_HttpParser;
    WebsocketRequest *ws_request;
    WebsocketRespond *ws_respond;
};

typedef unordered_map<uint32_t, CMsgConn*> WebSocketConnMap_t;

CMsgConn* FindWebSocketConnByHandle(uint32_t handle);
void MapConn(CMsgConn * handle);
void init_websocket_conn();

#endif /* __MSG_CONN_CONN_H__ */
