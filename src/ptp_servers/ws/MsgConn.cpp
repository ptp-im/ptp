#include "EncDec.h"
#include "DBServConn.h"
#include "MsgConn.h"
#include "HttpQuery.h"
#include <memory>
#include <string>
#include <cstdlib>
#include <cinttypes>
#include "IM.Server.pb.h"
#include "PTP.Other.pb.h"
#include "ImUser.h"
#include "rsa.h"
#include "base64.h"
#include "secp256k1_helpers.h"
#include "helpers.h"
#include "LoginServConn.h"
#include "aes_encryption.h"

using namespace std;
using namespace IM::BaseDefine;

static CHandlerMap* s_handler_map;

static WebSocketConnMap_t g_websocket_conn_map;

// conn_handle 从0开始递增，可以防止因socket handle重用引起的一些冲突
static uint32_t g_conn_handle_generator = 0;

static uint64_t	g_last_stat_tick;	// 上次显示丢包率信息的时间
static uint32_t g_up_msg_total_cnt = 0;		// 上行消息包总数
static uint32_t g_up_msg_miss_cnt = 0;		// 上行消息包丢数
static uint32_t g_down_msg_total_cnt = 0;	// 下行消息包总数
static uint32_t g_down_msg_miss_cnt = 0;	// 下行消息丢包数

static bool g_log_msg_toggle = true;	// 是否把收到的MsgData写入Log的开关，通过kill -SIGUSR2 pid 打开/关闭

static void signal_handler_usr1(int sig_no)
{
    if (sig_no == SIGUSR1) {
        log("receive SIGUSR1 ");
        g_up_msg_total_cnt = 0;
        g_up_msg_miss_cnt = 0;
        g_down_msg_total_cnt = 0;
        g_down_msg_miss_cnt = 0;
    }
}

static void signal_handler_usr2(int sig_no)
{
    if (sig_no == SIGUSR2) {
        log("receive SIGUSR2 ");
        g_log_msg_toggle = !g_log_msg_toggle;
    }
}

static void signal_handler_hup(int sig_no)
{
    if (sig_no == SIGHUP) {
        log("receive SIGHUP exit... ");
        exit(0);
    }
}

void MapConn(CMsgConn* handle){
    g_websocket_conn_map.insert(make_pair(handle->GetConnHandle(), handle));
}

CMsgConn *FindWebSocketConnByHandle(uint32_t conn_handle) {
    CMsgConn *pConn = NULL;
    WebSocketConnMap_t::iterator it = g_websocket_conn_map.find(conn_handle);
    if (it != g_websocket_conn_map.end()) {
        pConn = it->second;
    }

    return pConn;
}

void CMsgConn::SendUserStatusUpdate(uint32_t user_status)
{
    if (!m_bOpen) {
        return;
    }

    CImUser* pImUser = CImUserManager::GetInstance()->GetImUserById(GetUserId());
    if (!pImUser) {
        return;
    }
    // 只有上下线通知才通知LoginServer
    if (user_status == ::IM::BaseDefine::USER_STATUS_ONLINE) {
        IM::Server::IMUserCntUpdate msg;
        msg.set_user_action(USER_CNT_INC);
        msg.set_user_id(pImUser->GetUserId());
        CImPdu pdu;
        pdu.SetPBMsg(&msg);
        pdu.SetServiceId(SID_OTHER);
        pdu.SetCommandId(CID_OTHER_USER_CNT_UPDATE);
        send_to_all_login_server(&pdu);

        IM::Server::IMUserStatusUpdate msg2;
        msg2.set_user_status(::IM::BaseDefine::USER_STATUS_ONLINE);
        msg2.set_user_id(pImUser->GetUserId());
        msg2.set_client_type((::IM::BaseDefine::ClientType)m_client_type);
        CImPdu pdu2;
        pdu2.SetPBMsg(&msg2);
        pdu2.SetServiceId(SID_OTHER);
        pdu2.SetCommandId(CID_OTHER_USER_STATUS_UPDATE);

        send_to_all_route_server(&pdu2);
    } else if (user_status == ::IM::BaseDefine::USER_STATUS_OFFLINE) {
        IM::Server::IMUserCntUpdate msg;
        msg.set_user_action(USER_CNT_DEC);
        msg.set_user_id(pImUser->GetUserId());
        CImPdu pdu;
        pdu.SetPBMsg(&msg);
        pdu.SetServiceId(SID_OTHER);
        pdu.SetCommandId(CID_OTHER_USER_CNT_UPDATE);
        send_to_all_login_server(&pdu);

        IM::Server::IMUserStatusUpdate msg2;
        msg2.set_user_status(::IM::BaseDefine::USER_STATUS_OFFLINE);
        msg2.set_user_id(pImUser->GetUserId());
        msg2.set_client_type((::IM::BaseDefine::ClientType)m_client_type);
        CImPdu pdu2;
        pdu2.SetPBMsg(&msg2);
        pdu2.SetServiceId(SID_OTHER);
        pdu2.SetCommandId(CID_OTHER_USER_STATUS_UPDATE);
        send_to_all_route_server(&pdu2);
    }
}

void CMsgConn::SetClientInfo( string client_id,bool is_intel,string os_name,string os_version,string browser_name,string browser_version)
{
    m_client_id = client_id;
    m_is_intel = is_intel;
    m_os_name = os_name;
    m_os_version = os_version;
    m_browser_name = browser_name;
    m_browser_version = browser_version;
}

void CMsgConn::SetProperty( string client_version, uint32_t client_type,uint32_t online_status)
{
    m_client_version = client_version;
    m_client_type = client_type;
    m_online_status = online_status;
}

void websocketconn_callback(void *callback_data, uint8_t msg, uint32_t handle, uint32_t uParam, void *pParam) {
    NOTUSED_ARG(uParam);
    NOTUSED_ARG(pParam);

    // convert void* to uint32_t, oops
    uint32_t conn_handle = *((uint32_t *) (&callback_data));
    CMsgConn *pConn = FindWebSocketConnByHandle(conn_handle);
    if (!pConn) {
        return;
    }

    switch (msg) {
        case NETLIB_MSG_READ:
            pConn->OnRead();
            break;
        case NETLIB_MSG_WRITE:
            pConn->OnWrite();
            break;
        case NETLIB_MSG_CLOSE:
            pConn->OnClose();
            break;
        default:
            log("!!!httpconn_callback websocketconn_callbackerror msg: %d ", msg);
            break;
    }
}

void websocket_conn_timer_callback(void *callback_data, uint8_t msg, uint32_t handle, void *pParam) {
    CMsgConn *pConn = NULL;
    WebSocketConnMap_t::iterator it, it_old;
    uint64_t cur_time = get_tick_count();

    for (it = g_websocket_conn_map.begin(); it != g_websocket_conn_map.end();) {
        it_old = it;
        it++;

        pConn = it_old->second;
        pConn->OnTimer(cur_time);
    }
}

void init_websocket_conn() {

    g_last_stat_tick = get_tick_count();
    signal(SIGUSR1, signal_handler_usr1);
    signal(SIGUSR2, signal_handler_usr2);
    signal(SIGHUP, signal_handler_hup);

    netlib_register_timer(websocket_conn_timer_callback, NULL, 5000);

}

//////////////////////////
CMsgConn::CMsgConn() {
    s_handler_map = CHandlerMap::getInstance();
    m_busy = false;
    m_testing_require_pdu = false;
    m_conn_handle = NETLIB_INVALID_HANDLE;
    m_state = WS_STATE_UNCONNECTED;
    ws_request = new Websocket_Request();
    ws_respond = new Websocket_Respond();
    m_msg_cnt_per_sec = 0;
    m_last_send_tick = m_last_recv_tick = get_tick_count();
    m_conn_handle = ++g_conn_handle_generator;
    if (m_conn_handle == 0) {
        m_conn_handle = ++g_conn_handle_generator;
    }
    //log("CHttpConn, handle=%u ", m_conn_handle);
}

CMsgConn::~CMsgConn() {
    log("~CMsgConn, handle=%u ", m_conn_handle);
}


void CMsgConn::AddToSendList(uint32_t msg_id, uint32_t from_id)
{
    //log("AddSendMsg, seq_no=%u, from_id=%u ", seq_no, from_id);
    msg_ack_t msg;
    msg.msg_id = msg_id;
    msg.from_id = from_id;
    msg.timestamp = get_tick_count();
    m_send_msg_list.push_back(msg);

    g_down_msg_total_cnt++;
}

void CMsgConn::DelFromSendList(uint32_t msg_id, uint32_t from_id)
{
    //log("DelSendMsg, seq_no=%u, from_id=%u ", seq_no, from_id);
    for (list<msg_ack_t>::iterator it = m_send_msg_list.begin(); it != m_send_msg_list.end(); it++) {
        msg_ack_t msg = *it;
        if ( (msg.msg_id == msg_id) && (msg.from_id == from_id) ) {
            m_send_msg_list.erase(it);
            break;
        }
    }
}

int CMsgConn::Send(void *data, int len) {
    int ret = netlib_send(m_conn_handle, data, len);
    return len;
}


void CMsgConn::SendPdu(CImPdu * pdu) {
    if(this){
        if(!m_testing_require_pdu){
            auto pPdu = CImPdu::ReadPdu(pdu->GetBuffer(), pdu->GetLength());
            if(pPdu->GetReversed()){
                log_debug("Send len:%d data:%s ",pdu->GetLength(), bytes_to_hex_string(pdu->GetBuffer(),pdu->GetLength()).c_str());
                unsigned char shared_secret[32];
                unsigned char iv[16];
                unsigned char aad[16];
                string shareKey = this->GetShareKey();
                string ivHex = this->GetIv();
                string aadHex = this->GetAad();
                log_debug("org pdu: %s", bytes_to_hex_string(pdu->GetBuffer(),pdu->GetLength()).c_str());
                memcpy(shared_secret,hex_to_string(shareKey.substr(2)).data(),32);
                memcpy(iv,hex_to_string(ivHex.substr(2)).data(),16);
                memcpy(aad,hex_to_string(aadHex.substr(2)).data(),16);

                unsigned char cipherData[1024*1024];
                int cipherDataLen = aes_gcm_encrypt(
                        pPdu->GetBodyData(),
                        (int)pPdu->GetBodyLength(),
                        shared_secret,
                        iv,aad,
                        cipherData);

                pdu->SetPBMsg(cipherData,cipherDataLen);
                pdu->SetServiceId(pPdu->GetServiceId());
                pdu->SetCommandId(pPdu->GetCommandId());
                pdu->SetSeqNum(pPdu->GetSeqNum());
                pdu->SetReversed(pPdu->GetReversed());
            }

            if(m_is_websocket){
                SendBufMessageToWS(pdu->GetBuffer(),(int)pdu->GetLength());
            }else{
                Send(pdu->GetBuffer(),(int)pdu->GetLength());
            }
        }else{
            uint32_t pdu_len = 0;
            if (!CImPdu::IsPduAvailable(pdu->GetBuffer(), pdu->GetLength(), pdu_len)){
                log_error("IsPduAvailable: false");
                Close();
            }
            m_sendPdu = CImPdu::ReadPdu(pdu->GetBuffer(), pdu_len);
        }
    }

}

void CMsgConn::SendBufMessageToWS( void *data, int messageLength ) {
    std::string outFrame;
    if (messageLength > 20000) {
        return;
    }
    uint8_t payloadFieldExtraBytes = (messageLength <= 0x7d) ? 0 : 2;
    // header: 2字节, mask位设置为0(不加密), 则后面的masking key无须填写, 省略4字节
    uint8_t frameHeaderSize = 2 + payloadFieldExtraBytes;
    auto *frameHeader = new uint8_t[frameHeaderSize];

    memset(frameHeader, 0, frameHeaderSize);

    // fin位为1, 扩展位为0, 操作位为frameType
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

void CMsgConn::toFrameDataPkt(const std::string &data) {
    std::string outFrame;
    uint32_t messageLength = data.size();
    if (messageLength > 20000) {
        return;
    }
    uint8_t payloadFieldExtraBytes = (messageLength <= 0x7d) ? 0 : 2;
    // header: 2字节, mask位设置为0(不加密), 则后面的masking key无须填写, 省略4字节
    uint8_t frameHeaderSize = 2 + payloadFieldExtraBytes;
    uint8_t *frameHeader = new uint8_t[frameHeaderSize];
    memset(frameHeader, 0, frameHeaderSize);
    // fin位为1, 扩展位为0, 操作位为frameType
    frameHeader[0] = static_cast<uint8_t>(0x80 | 0x1);
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
    memcpy(frame + frameHeaderSize, &data, messageLength);
    //frame[frameSize] = '\0';
    Send(frame, frameSize);
    delete[] frame;
}

void CMsgConn::OnWrite() {
}

void CMsgConn::OnClose() {
    Close();
}

void CMsgConn::OnTimer(uint64_t curr_tick) {
    m_msg_cnt_per_sec = 0;
    if (curr_tick > m_last_recv_tick + WEBSOCKET_CONN_TIMEOUT) {
        log("WebSocketConn timeout, handle=%d ", m_conn_handle);
        Close();
    }
}

void CMsgConn::OnWriteCompelete() {
    // Close();
}

void CMsgConn::Close() {
    CImUser *pImUser = CImUserManager::GetInstance()->GetImUserById(GetUserId());
    if (pImUser) {
        pImUser->BroadcastDevicesPduOnClose(this);
        SendUserStatusUpdate(::IM::BaseDefine::USER_STATUS_OFFLINE);
        pImUser->DelMsgConn(GetConnHandle());
        pImUser->DelUnValidateMsgConn(this);
        if (pImUser->IsMsgConnEmpty()) {
            CImUserManager::GetInstance()->RemoveImUser(pImUser);
        }
    }

    pImUser = CImUserManager::GetInstance()->GetImUserByAddressHex(GetAddressHex());
    if (pImUser) {
        pImUser->DelUnValidateMsgConn(this);
        if (pImUser->IsMsgConnEmpty()) {
            CImUserManager::GetInstance()->RemoveImUser(pImUser);
        }
    }

    if (m_state != WS_STATE_CLOSED) {
        m_state = WS_STATE_CLOSED;

        g_websocket_conn_map.erase(m_conn_handle);
        netlib_close(m_conn_handle);
    }
}

void CMsgConn::OnConnect(net_handle_t handle) {
    log("new client OnConnect, handle=%d", handle);

    isHandshark = false;
    m_conn_handle = handle;

    m_state = WS_STATE_CONNECTED;
    g_websocket_conn_map.insert(make_pair(m_conn_handle, this));

    netlib_option(handle, NETLIB_OPT_SET_CALLBACK, (void *) websocketconn_callback);
    netlib_option(handle, NETLIB_OPT_SET_CALLBACK_DATA, reinterpret_cast<void *>(m_conn_handle));
    netlib_option(handle, NETLIB_OPT_GET_REMOTE_IP, (void *) &m_peer_ip);
}

bool CMsgConn::is_login_server_ok()
{
    return true;
}

void CMsgConn::OnRead() {

    m_in_buf = *new CSimpleBuffer();
    int i = 0;
    for (;;) {
        i++;
        uint32_t free_buf_len = m_in_buf.GetAllocSize() - m_in_buf.GetWriteOffset();
        if (free_buf_len < READ_BUF_SIZE)
            m_in_buf.Extend(READ_BUF_SIZE);
        int ret = netlib_recv(m_conn_handle, m_in_buf.GetBuffer() + m_in_buf.GetWriteOffset(), READ_BUF_SIZE);
        //log_debug("i#%d,ret=%d,free_buf_len=%d,READ_BUF_SIZE=%d,m_in_buf.GetWriteOffset()=%d",i,ret,free_buf_len,READ_BUF_SIZE,m_in_buf.GetWriteOffset());
        if (ret <= 0){
            i = 0;
            break;
        }
        m_in_buf.IncWriteOffset(ret);
        m_last_recv_tick = get_tick_count();

    }

    char *in_buf = (char *) m_in_buf.GetBuffer();
    uint32_t buf_len = m_in_buf.GetWriteOffset();
    if(buf_len != 16 && buf_len != 22){
        log_debug("read buf_len: %d", buf_len);
    }
    uint32_t pdu_len = 0;
    if (CImPdu::IsPduAvailable(m_in_buf.GetBuffer(), m_in_buf.GetWriteOffset(),pdu_len)){
        m_is_websocket = false;
        int offset = 0;
        while(true){
            if(buf_len != 16 && buf_len != 22){
                log_debug("pdu_len=%d,offset=%d", pdu_len,offset);
            }
            HandlePduBuf(m_in_buf.GetBuffer() + offset, pdu_len);
            if(buf_len == offset + pdu_len){
                break;
            }else{
                offset += pdu_len;
                pdu_len = CByteStream::ReadUint32(m_in_buf.GetBuffer() + offset);
                if(pdu_len < IM_PDU_HEADER_LEN || offset + pdu_len > buf_len){
                    break;
                }
            }
        }

    }else{
        m_is_websocket = true;
        if (!isHandshark) {
            strcpy(buff_, in_buf);
            char *b = "Connection: Upgrade";
            if (strstr(in_buf, b) != NULL) {
                char *c = "Sec-WebSocket-Key:";
                if (strstr(in_buf, c) != NULL) {
                    char request[1024] = {};
                    ws_respond->fetch_http_info(buff_);
                    ws_respond->parse_str(request);
                    //发送握手回复
                    Send(request, (uint32_t) strlen(request));
                    isHandshark = true;
                    m_state = WS_STATE_HANDSHARK;
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
            memset(in_buf, 0, sizeof(in_buf));
        }
        if (ws_request->opcode_ == 0x1) {
            Close();
        }if (ws_request->opcode_ == 0x2) {
            while (1){
                auto buf = reinterpret_cast<uchar_t *>(ws_request->payload_);
                auto len = ws_request->payload_length_;
                if (!CImPdu::IsPduAvailable(buf, len, pdu_len)){
                    log_error("IsPduAvailable: false");
                    break;
                }
                if(pdu_len == 0){
                    log_error("pdu_len: 0");
                    break;
                }
                HandlePduBuf(buf, pdu_len);
                break;
            }

        } else if (ws_request->opcode_ == 0x8) {//连接关闭
            Close();
        }
    }

}

void CMsgConn::HandlePduBuf(uchar_t* pdu_buf, uint32_t pdu_len)
{
    CImPdu* pPdu = NULL;
    try
    {
        while (true){
            pPdu = CImPdu::ReadPdu(pdu_buf, pdu_len);
            if (pPdu->GetCommandId() == PTP::Common::CID_HeartBeatNotify) {
                //log_debug("HeartBeatNotify");
                PTP::Other::HeartBeatNotify msg;
                CImPdu pdu_rsp;
                pdu_rsp.SetPBMsg(&msg);
                pdu_rsp.SetServiceId(PTP::Common::S_OTHER);
                pdu_rsp.SetCommandId(PTP::Common::CID_HeartBeatNotify);
                pdu_rsp.SetSeqNum(pPdu->GetSeqNum());
                SendPdu(&pdu_rsp);
                break;
            }

            if(pPdu->GetCommandId() != PTP::Common::CID_AuthCaptchaReq){
                if (pPdu->GetCommandId() != PTP::Common::CID_AuthLoginReq && !this->IsOpen()) {
                    log_error("HandlePdu, wrong msg. ");
                    throw CPduException(pPdu->GetServiceId(), pPdu->GetCommandId(), ERROR_CODE_WRONG_SERVICE_ID, "HandlePdu error, user not login. ");
                    break;
                }
            }

            log("handle cid: %d, r:%d", pPdu->GetCommandId(), pPdu->GetReversed());
            if(pPdu->GetCommandId() != PTP::Common::CID_AuthCaptchaReq
                && pPdu->GetCommandId() != PTP::Common::CID_AuthLoginReq
                && pPdu->GetBodyLength() > 0
                   && pPdu->GetReversed() == 1
                ){
                auto cipherData = pPdu->GetBodyData();
                auto cipherDataLen = pPdu->GetBodyLength();

                unsigned char shared_secret[32];
                unsigned char iv[16];
                unsigned char aad[16];
                string shareKey = this->GetShareKey();
                string ivHex = this->GetIv();
                string aadHex = this->GetAad();

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
                    pPdu->SetServiceId(pPdu->GetServiceId());
                    pPdu->SetCommandId(pPdu->GetCommandId());
                    pPdu->SetSeqNum(pPdu->GetSeqNum());
                    pPdu->SetReversed(pPdu->GetReversed());
                    pPdu = CImPdu::ReadPdu(pPdu->GetBuffer(), pPdu->GetLength());
                    memset(decData, 0, sizeof(decData));

                }else{
                    log_error("error decrypt body,cid:%d",pPdu->GetCommandId() );
                    break;
                }
                log("pdu_len: %d", pdu_len);
            }

            pdu_handler_t handler = s_handler_map->GetHandler(pPdu->GetCommandId());
            if (handler) {
                handler(pPdu, this->GetConnHandle());
            } else {
                log("no handler for packet type: %d", pPdu->GetCommandId());
            }
            break;
        }
    } catch (CPduException& ex) {
        log_error("!!!catch exception, sid=%u, cid=%u, err_code=%u, err_msg=%s, close the connection ",
                  ex.GetServiceId(), ex.GetCommandId(), ex.GetErrorCode(), ex.GetErrorMsg());
        Close();
    }

    if (pPdu) {
        delete pPdu;
        pPdu = NULL;
    }
}