#include "ClientConn.h"
#include <string>

#define MAX_RECONNECT_CNT	32
#define MIN_RECONNECT_CNT	4

static ConnectiosManagerDelegate*   m_delegate;

typedef unordered_map<uint32_t , CClientConn*> AccountConnMap_t;

static ConnectionState connectionState;

static ConnMap_t g_msg_conn_map;

static AccountConnMap_t g_account_msg_conn_map;

static uint32_t		current_account_id = 0;
static uint32_t		connecting_cnt = 0;
static uint32_t		timer_cnt = 0;

static uint32_t	idle_cnt = 0;
static uint32_t	reconnect_cnt = 0;

static void msg_conn_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
    if(timer_cnt++ > 100){
        timer_cnt = 0;
    }
    uint64_t cur_time = get_tick_count();
    if(connectionState == ConnectionStateConnected){
        for (auto it = g_account_msg_conn_map.begin(); it != g_account_msg_conn_map.end(); ) {
            if(it->first == get_current_account_id()){
                CClientConn*  pConn = (CClientConn*)it->second;
                if (pConn != NULL && pConn->IsOpen()) {
                    if(timer_cnt > 0 && timer_cnt % 29 == 0 ){
                        DEBUG_D("on_timer:%d",get_current_account_id());
                    }
                    pConn->OnTimer(cur_time);
                }
                break;
            }
        }
    }

    if(reconnect_cnt > 0){
        if(connectionState == ConnectionStateClosed){
            idle_cnt++;
        }
        if(connectionState == ConnectionStateConnecting){
            connecting_cnt++;
        }
        if(connecting_cnt > 30){
            connecting_cnt = 0;
            close_msg_conn(get_current_account_id());
        }
        if(idle_cnt >= reconnect_cnt && connectionState == ConnectionStateClosed){
            DEBUG_D("reconnect cnt=%d,idle_cnt:%d", reconnect_cnt,idle_cnt);
            init_msg_conn(get_current_account_id(),false);
        }
    }
}
void init_msg_conn(uint32_t accountId){
    init_msg_conn(accountId,true);
}

void init_msg_conn(uint32_t accountId,bool register_timer = true)
{
    CClientConn *pConn = new CClientConn();
    string		server_ip = MSG_SERVER_IP_1;
    uint16_t	server_port = MSG_SERVER_PORT_1;
    if(register_timer){
        DEBUG_D("initMsgConn accountId:%d,server_ip:%s,server_port:%d", accountId,server_ip.c_str(),server_port);
    }
    pConn->Connect(server_ip.c_str(), server_port, 0,accountId);

    if(register_timer){
        netlib_register_timer(msg_conn_timer_callback, NULL, 1000);
    }
    netlib_eventloop();
}

void close_msg_conn(uint32_t account_id){
    for (auto it = g_account_msg_conn_map.begin(); it != g_account_msg_conn_map.end(); ) {
        if(it->first == account_id){
            CClientConn *pConn = (CClientConn*)it->second;
            if (pConn != NULL && pConn->IsOpen()) {
                pConn->Close();
                break;
            }
        }
    }
}

uint32_t get_current_account_id(){
    return current_account_id;
}

void set_current_account_id(uint32_t account_id){
    current_account_id = account_id;
}

void set_delegate(ConnectiosManagerDelegate* delegate){
    m_delegate = delegate;
}

CClientConn* get_conn(uint32_t account_id)
{
    uint32_t i = 0;
    CClientConn* pMsgConn = NULL;
    for (auto it = g_account_msg_conn_map.begin(); it != g_account_msg_conn_map.end(); it++) {
        if(account_id == it->first){
            pMsgConn = (CClientConn*)it->second;
            return pMsgConn;
        }
    }
    return pMsgConn;
}

CClientConn::CClientConn()
{
    m_bOpen = false;

}

CClientConn::~CClientConn()
{

}

void CClientConn::reset_reconnect()
{
    if(reconnect_cnt > 0){
        reconnect_cnt = 1;
    }
}

void CClientConn::Connect(const char* server_ip, uint16_t server_port, uint32_t serv_idx, uint32_t account_id)
{
    connectionState = ConnectionStateConnecting;
    CClientConn::onConnectionStateChanged(connectionState,account_id);
    m_serv_idx = serv_idx;
    m_accountId = account_id;
    m_handle = netlib_connect(server_ip, server_port, imconn_callback, (void*)&g_msg_conn_map);
    DEBUG_D("connecting %s:%d aid:%d m_handle:%d", server_ip, server_port,account_id,m_handle);

    if (m_handle != NETLIB_INVALID_HANDLE) {
        g_msg_conn_map.insert(make_pair(m_handle, this));
        g_account_msg_conn_map.insert(make_pair(account_id, this));
    }
}

void CClientConn::Close()
{
    connectionState = ConnectionStateClosed;
    CClientConn::onConnectionStateChanged(ConnectionStateClosed,GetAccountId());

    if(reconnect_cnt == 0){
        reconnect_cnt = 1;
    }else{
        reconnect_cnt *= 2;
    }

    if (reconnect_cnt > MAX_RECONNECT_CNT) {
        reconnect_cnt = 1;
        idle_cnt = 0;
    }
    DEBUG_D("closed handle=%d,accountId:%d,idle_cnt:%d,reconnect aft: %ds", m_handle,GetAccountId(),idle_cnt,reconnect_cnt);


    if (m_handle != NETLIB_INVALID_HANDLE) {
        netlib_close(m_handle);
        g_msg_conn_map.erase(m_handle);
        g_account_msg_conn_map.erase(GetAccountId());
    }
    ReleaseRef();
}

void CClientConn::OnConfirm()
{
    m_bOpen = true;
    reconnect_cnt = 0;
    idle_cnt = 0;
    connecting_cnt = 0;
    DEBUG_D(">>[Connected]<<,aId:%d,current:%d,idle_cnt:%d,reconnect_cnt:%d",
            GetAccountId(),get_current_account_id(),idle_cnt,reconnect_cnt);

    connectionState = ConnectionStateConnected;
    CClientConn::onConnectionStateChanged(ConnectionStateConnected,GetAccountId());
}

void CClientConn::OnClose()
{
    Close();
}

void CClientConn::OnTimer(uint64_t curr_tick)
{
    if(get_current_account_id() != GetAccountId()){
        Close();
    }else{
        if (curr_tick > m_last_send_tick + CLIENT_HEARTBEAT_INTERVAL && connectionState == ConnectionLogged) {
            ImPdu pdu;
            pdu.SetPBMsg(nullptr);
            pdu.SetServiceId(7);
            pdu.SetCommandId(1793);
            SendPdu(&pdu);
            SendPduBuf(pdu.GetBuffer(),pdu.GetLength());
            DEBUG_D("heart beat... %d",CLIENT_HEARTBEAT_INTERVAL);
        }

        if (curr_tick > m_last_recv_tick + MOBILE_CLIENT_TIMEOUT) {
            DEBUG_D("conn to msg server timeout %d",MOBILE_CLIENT_TIMEOUT);
            Close();
        }
    }
}

int CClientConn::SendPduBuf(uint8_t* pduBytes,uint32_t size){
    auto pPdu = CImPdu::ReadPdu(pduBytes, size);
    delete pduBytes;
    DEBUG_D("pdu send n cid=%d len=%d ",pPdu->GetCommandId(),pPdu->GetLength());
    if(pPdu->GetCommandId() == 1281){
        DEBUG_D("CID_FileImgUploadReq_VALUE");
    }
    int res;
    if(pPdu->GetReversed() == 1 && pPdu->GetBodyLength() > 0){
        unsigned char shared_secret[32];
        unsigned char iv[16];
        unsigned char aad[16];
        memcpy(shared_secret,hex_to_string(m_sharedKeyHex).data(),32);
        memcpy(iv,hex_to_string(m_ivHex).data(),16);
        memcpy(aad,hex_to_string(m_aadHex).data(),16);

        unsigned char cipherData[1024*1024];
        int cipherDataLen = aes_gcm_encrypt(
                pPdu->GetBodyData(),
                (int)pPdu->GetBodyLength(),
                shared_secret,
                iv,aad,
                cipherData);
        ImPdu pdu1;
        pdu1.SetPBMsg(cipherData,cipherDataLen);
        pdu1.SetServiceId(pPdu->GetServiceId());
        pdu1.SetCommandId(pPdu->GetCommandId());
        pdu1.SetSeqNum(pPdu->GetSeqNum());
        pdu1.SetReversed(pPdu->GetReversed());
        res = Send(pdu1.GetBuffer(), (int)pdu1.GetLength());
    }else{
        res = Send(pPdu->GetBuffer(), (int)pPdu->GetLength());
    }
    delete pPdu;
    pPdu = NULL;
    return res;
}

void CClientConn::HandlePdu(CImPdu* pPdu)
{
    AccountManager::getInstance(GetAccountId()).handlePdu(pPdu);
}


void CClientConn::onConnectionStateChanged(ConnectionState state, uint32_t accountId)
{
    m_delegate->onConnectionStateChanged(state,0);
}

void CClientConn::onNotify(NativeByteBuffer *buffer,int32_t instanceNum)
{
    if( nullptr != m_delegate){
        buffer->position(0);
        buffer->reuse();
        m_delegate->onNotify(buffer,instanceNum);
    }
}

