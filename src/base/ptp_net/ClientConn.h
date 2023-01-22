#ifndef __CLENT_CONN_H__
#define __CLENT_CONN_H__

#include "ptp_global/ImConn.h"
#include "ptp_global/Logger.h"
#include "ptp_crypto/secp256k1_helpers.h"
#include "ptp_crypto/crypto_helpers.h"
#include "ptp_crypto/aes_encryption.h"
#include "ptp_toolbox/data/bytes_array.h"

#include "AccountManager.h"
#include "Defines.h"
#include "NativeByteBuffer.h"
#include "BuffersStorage.h"

class CClientConn : public CImConn
{
public:
    CClientConn();
    virtual ~CClientConn();

    bool IsOpen() { return m_bOpen; }
    uint32_t GetAccountId(){return m_accountId;};

    void Connect(const char* server_ip, uint16_t server_port, uint32_t serv_idx, uint32_t account_id);
    virtual void Close();
    virtual void OnConfirm();
    virtual void OnClose();
    virtual void OnTimer(uint64_t curr_tick);
    virtual void HandlePdu(CImPdu* pPdu);
    int SendPduBuf(uint8_t* pduBytes,uint32_t size);
    static void reset_reconnect();
    static void HandlePduBuf(NativeByteBuffer *buff,uint32_t readCount,NativeByteBuffer *outBuf);
    static void onConnectionStateChanged(ConnectionState state, uint32_t accountId);
    static void onNotify(NativeByteBuffer *buffer,int32_t instanceNum);
private:
    string		m_ivHex;
    string		m_aadHex;
    string		m_sharedKeyHex;
    bool 		m_bOpen;
    uint32_t	m_accountId;
    uint32_t	m_serv_idx;
};

void set_delegate(ConnectiosManagerDelegate* delegate);
void set_current_account_id(uint32_t account_id);
uint32_t get_current_account_id();
void init_msg_conn(uint32_t account_id);
void init_msg_conn(uint32_t account_id,bool register_timer);
void close_msg_conn(uint32_t account_id);
CClientConn* get_conn(uint32_t accountId);

#endif
