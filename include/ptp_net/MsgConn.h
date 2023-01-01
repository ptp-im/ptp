#ifndef MSGCONN_H_
#define MSGCONN_H_

#include "ptp_common/imconn.h"
#include "ptp_common/ServInfo.h"

#ifdef ANDROID
#include <jni.h>
#include "../../tgnet/FileLog.h"
#include "../../tgnet/Defines.h"
#include "../../tgnet/NativeByteBuffer.h"
#else
#include "ptp_common/Logger.h"
#include "ptp_net/Defines.h"
#include "ptp_net/NativeByteBuffer.h"
#endif

#define NativeInvokeHeaderSize 16
#define NativeInvokeBoolSize 4
#define NativeInvokeIntSize 4
#define NativeInvokeDefaultSepNum 0

class CMsgConn : public CImConn
{
public:
    CMsgConn();
    virtual ~CMsgConn();

    bool IsOpen() { return m_bOpen; }
    uint32_t GetAccountId(){return m_accountId;};

    void Connect(const char* server_ip, uint16_t server_port, uint32_t serv_idx, uint32_t account_id);
    virtual void Close();

    int SendPdu(CImPdu* pdu);
    virtual void OnConfirm();
    virtual void OnClose();
    virtual void OnTimer(uint64_t curr_tick);

    virtual void HandlePdu(CImPdu* pPdu);
private:

private:
    bool 		m_bOpen;
    uint32_t	m_accountId;
    uint32_t	m_serv_idx;
};

void set_current_account_id(uint32_t account_id);
uint32_t get_current_account_id();
void init_msg_conn(uint32_t account_id);
void close_msg_conn(uint32_t account_id);
void reset_msg_conn();


CMsgConn* get_msg_conn(uint32_t accountId);

typedef unordered_map<uint32_t , serv_info_t*> ServerInfoMap_t;


enum NativeInvokeType {
    NativeInvokeType_HEART_BEAT = 1000,
    NativeInvokeType_SET_WORDS = 1001,
    NativeInvokeType_GET_WORDS = 1002,
    NativeInvokeType_PDU_RECV = 1003,
    NativeInvokeType_SIGN_AUTH_CAPTCHA = 1004,
    NativeInvokeType_GEN_WORDS = 1005,
    NativeInvokeType_PDU_SEND = 1006,
    NativeInvokeType_PDU_UPLOAD = 100601,
    NativeInvokeType_PDU_DOWNLOAD = 100602,
    NativeInvokeType_CHANGE_ACCOUNT = 1007,
    NativeInvokeType_SIGN_GROUP_MSG = 1008,
};


#endif
