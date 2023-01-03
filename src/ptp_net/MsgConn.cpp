#include "ptp_net/MsgConn.h"
#include "ptp_net/MsgConnManager.h"
#include "ptp_net/AccountManager.h"
#include "BuffersStorage.h"
#include <string>
#include "ptp_crypto/secp256k1_helpers.h"
#include "ptp_toolbox/data/bytes_array.h"
#include "ptp_crypto/ptp_helpers.h"
#include "ptp_crypto/aes_encryption.h"

#ifdef ANDROID
#include <jni.h>
#include "../../tgnet/FileLog.h"
#include "../../tgnet/Defines.h"
#include "../../tgnet/NativeByteBuffer.h"
#include "../../tgnet/BuffersStorage.h"
#else
#include "ptp_common/Logger.h"
#include "ptp_net/Defines.h"
#include "ptp_net/NativeByteBuffer.h"
#include "ptp_net/BuffersStorage.h"
#endif


#define MAX_RECONNECT_CNT	32
#define MIN_RECONNECT_CNT	4


typedef unordered_map<uint32_t , CMsgConn*> AccountConnMap_t;

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
                CMsgConn*  pConn = (CMsgConn*)it->second;
                if (pConn != NULL && pConn->IsOpen()) {
                    if(timer_cnt > 0 && timer_cnt % 10 == 0 ){
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
    CMsgConn *pConn = new CMsgConn();
    string		server_ip = MSG_SERVER_IP_1;
    uint16_t	server_port = MSG_SERVER_PORT_1;
    if(register_timer){
        DEBUG_D("initMsgConn accountId:%d,server_ip:%s,server_port:%d", accountId,server_ip.c_str(),server_port);
    }
    pConn->Connect(server_ip.c_str(), server_port, 0,accountId);

    if(register_timer){
        netlib_register_timer(msg_conn_timer_callback, NULL, 1000);
    }
}

void close_msg_conn(uint32_t account_id){
    uint64_t cur_time = get_tick_count();
    for (auto it = g_account_msg_conn_map.begin(); it != g_account_msg_conn_map.end(); ) {
        if(it->first == account_id){
            CMsgConn *pConn = (CMsgConn*)it->second;
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

CMsgConn* get_msg_conn(uint32_t account_id)
{
    uint32_t i = 0;
    CMsgConn* pMsgConn = NULL;
    for (auto it = g_account_msg_conn_map.begin(); it != g_account_msg_conn_map.end(); it++) {
        if(account_id == it->first){
            pMsgConn = (CMsgConn*)it->second;
            return pMsgConn;
        }
    }
    return pMsgConn;
}

CMsgConn::CMsgConn()
{
	m_bOpen = false;

}

CMsgConn::~CMsgConn()
{

}

void CMsgConn::reset_reconnect()
{
    if(reconnect_cnt > 0){
        reconnect_cnt = 1;
    }
}

void CMsgConn::Connect(const char* server_ip, uint16_t server_port, uint32_t serv_idx, uint32_t account_id)
{
    connectionState = ConnectionStateConnecting;
    MsgConnManager::onConnectionStateChanged(connectionState,account_id);
	m_serv_idx = serv_idx;
    m_accountId = account_id;
	m_handle = netlib_connect(server_ip, server_port, imconn_callback, (void*)&g_msg_conn_map);
    DEBUG_D("connecting %s:%d aid:%d m_handle:%d", server_ip, server_port,account_id,m_handle);

	if (m_handle != NETLIB_INVALID_HANDLE) {
       g_msg_conn_map.insert(make_pair(m_handle, this));
       g_account_msg_conn_map.insert(make_pair(account_id, this));
	}
}

void CMsgConn::Close()
{
    connectionState = ConnectionStateClosed;
    MsgConnManager::onConnectionStateChanged(ConnectionStateClosed,GetAccountId());

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

void CMsgConn::OnConfirm()
{
	m_bOpen = true;
    reconnect_cnt = 0;
    idle_cnt = 0;
    connecting_cnt = 0;
    DEBUG_D(">>[Connected]<<,aId:%d,current:%d,idle_cnt:%d,reconnect_cnt:%d",
            GetAccountId(),get_current_account_id(),idle_cnt,reconnect_cnt);

    connectionState = ConnectionStateConnected;
    MsgConnManager::onConnectionStateChanged(ConnectionStateConnected,GetAccountId());
}

void CMsgConn::OnClose()
{
	Close();
}

void CMsgConn::OnTimer(uint64_t curr_tick)
{
    if(get_current_account_id() != GetAccountId()){
        Close();
    }else{
        if (curr_tick > m_last_send_tick + CLIENT_HEARTBEAT_INTERVAL && connectionState == ConnectionLogged) {
            CImPdu pdu;
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

int CMsgConn::SendPduBuf(uint8_t* pduBytes,uint32_t size){
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
        CImPdu pdu1;
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

void CMsgConn::HandlePduBuf(NativeByteBuffer *buff, uint32_t readCount,NativeByteBuffer *outBuf){
    buff->position(0);
    uint8_t bb[1024];
    buff->copyBuffer(0,readCount,bb);
    uint32_t pdu_len = CByteStream::ReadUint32(bb);
    DEBUG_D("data: %s, len: %d", ptp_toolbox::data::bytes_to_hex(bb,pdu_len).c_str(),pdu_len);
    auto pPdu = CImPdu::ReadPdu(bb, pdu_len);
    DEBUG_D("HandlePdu GetCommandId: %d,len:%d",pPdu->GetCommandId(),pdu_len);

    if(readCount < pdu_len){
        outBuf->position(0);
        outBuf->writeInt32(0);
        outBuf->position(0);
    }else{
        if(pPdu->GetCommandId() == 1793){//heart beat
            return;
        }
        NativeByteBuffer *buffer;
        if(pPdu->GetReversed() == 1 ){
//            if(connectionState != ConnectionLogged){
//                connectionState = ConnectionLogged;
//                MsgConnManager::onConnectionStateChanged(connectionState,GetAccountId());
//            }
//
//            unsigned char shared_secret[32];
//            unsigned char iv[16];
//            unsigned char aad[16];
//            memcpy(shared_secret,hex_to_string(m_sharedKeyHex).data(),32);
//            memcpy(iv,hex_to_string(m_ivHex).data(),16);
//            memcpy(aad,hex_to_string(m_aadHex).data(),16);
//
//            auto cipherData = pPdu->GetBodyData();
//            auto cipherDataLen = pPdu->GetBodyLength();
//
//            unsigned char decData[1024];
//            int decLen = aes_gcm_decrypt(
//                    cipherData, (int)cipherDataLen,
//                    shared_secret, iv,
//                    aad,
//                    decData);
//            //DEBUG_D("decLen: %d",decLen);
//            //DEBUG_D("decData: %s",ptp_toolbox::data::bytes_to_hex(decData,decLen).c_str());
//
//            if (decLen > 0) {
//                CImPdu pdu;
//                pdu.SetPBMsg(decData,decLen);
//                pdu.SetServiceId(pPdu->GetServiceId());
//                pdu.SetCommandId(pPdu->GetCommandId());
//                pdu.SetSeqNum(pPdu->GetSeqNum());
//                pdu.SetReversed(0);
//                pPdu = CImPdu::ReadPdu(pdu.GetBuffer(), pdu.GetLength());
//                memset(decData, 0, sizeof(decData));
//                len = pPdu->GetLength();
//                //DEBUG_D("len: %d",len);
//                //DEBUG_D("GetBodyLength: %d",pPdu->GetBodyLength());
//                //DEBUG_D("GetBodyData: %s", bytes_to_hex_string(pPdu->GetBodyData(),pPdu->GetBodyLength()).c_str());
//            }else{
//                DEBUG_E("error decrypt body,cid:%d",pPdu->GetCommandId() );
//                return;
//            }
        }

        if(pPdu->GetCommandId() == 262){//auth captcha
//            auto buf = pPdu->GetBodyData();
//            unsigned char captchaBuff[6] = { 0 };
//            memcpy(captchaBuff,buf + 2,6);
//
//            unsigned char addressBuff[20] = { 0 };
//            memcpy(addressBuff,buf + 2 + 6 + 2,20);
//            //DEBUG_D("address: %s", ptp_toolbox::data::bytes_to_hex(addressBuff,20).c_str());
//
//            unsigned char ivBuf[16] = { 0 };
//            memcpy(ivBuf,buf + 2 + 6 + 2 + 20 + 2, 16);
//            m_ivHex = ptp_toolbox::data::bytes_to_hex(ivBuf,16);
//            //DEBUG_D("iv: %s", ptp_toolbox::data::bytes_to_hex(ivBuf,16).c_str());
//
//            unsigned char aadBuff[16] = { 0 };
//
//            memcpy(aadBuff,buf + 2 + 6 + 2 + 20 + 2 + 16 + 2, 16);
//            m_aadHex = ptp_toolbox::data::bytes_to_hex(aadBuff,16);
//            //DEBUG_D("aad: %s", ptp_toolbox::data::bytes_to_hex(aadBuff,16).c_str());
//
//            unsigned char sign65Buf[65] = { 0 };
//            memcpy(sign65Buf,buf + 2 + 6 + 2 + 20 + 2 + 16 + 2 + 16 + 2, 65);
//            //DEBUG_D("signOut65: %s", ptp_toolbox::data::bytes_to_hex(sign65Buf,65).c_str());
//
//            std::string captcha((char*) captchaBuff,6);
//            //DEBUG_D("captcha: %s", captcha.c_str());
//
//            string srv_address_hex;
//            unsigned char pub_key_33[33] = {0};
//            string msg_data = format_eth_msg_data(captcha);
//            auto ret = recover_pub_key_from_sig_65(sign65Buf, msg_data, pub_key_33,srv_address_hex);
//            if(!ret){
//                DEBUG_E("recover_pub_key_from_sig_65 error!");
//                return;
//            }
//
//            DEBUG_D("srv_address_hex: %s", srv_address_hex.c_str());
//
//            if(srv_address_hex.substr(2) != ptp_toolbox::data::bytes_to_hex(addressBuff,20)){
//                DEBUG_E("srv_address_hex error!");
//                return;
//            }
//
//            unsigned char shared_secret[32];
//            secp256k1_pubkey pub_key_raw;
//
//            secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
//            ret = secp256k1_ec_pubkey_parse(ctx,&pub_key_raw,pub_key_33,33);
//            secp256k1_context_destroy(ctx);
//            if(!ret){
//                DEBUG_E("parse pub_key_raw error");
//                return;
//            }
//            uint32_t accountId = GetAccountId();
//            AccountManager::getInstance(accountId).upsertEntropy();
//            AccountManager::getInstance(accountId).createShareKey(&pub_key_raw,shared_secret);
//
//            //DEBUG_D("shared_secret: %s", ptp_toolbox::data::bytes_to_hex(shared_secret,32).c_str());
//            m_sharedKeyHex = ptp_toolbox::data::bytes_to_hex(shared_secret,32);
//            string address = AccountManager::getInstance(accountId).getAccountAddress();
//            DEBUG_D("address: %s",address.c_str());
//            string sign = AccountManager::getInstance(accountId).signMessage(captcha);
//            auto *sizeCalculator = new NativeByteBuffer(true);
//            sizeCalculator->clearCapacity();
//            sizeCalculator->writeInt32(NativeInvokeHeaderSize);
//            sizeCalculator->writeInt32(NativeInvokeType_SIGN_AUTH_CAPTCHA);
//            sizeCalculator->writeInt32(0);
//            sizeCalculator->writeInt32((int32_t)GetAccountId());
//            sizeCalculator->writeString(captcha);
//            sizeCalculator->writeString(address);
//            auto* signBytes = (uchar_t *) sign.data();
//            sizeCalculator->writeBytes(signBytes,sign.size());
//            uint32_t size = sizeCalculator->capacity();
//            buffer = BuffersStorage::getInstance().getFreeBuffer(size);
//            buffer->writeInt32(size);
//            buffer->writeInt32(NativeInvokeType_SIGN_AUTH_CAPTCHA);
//            buffer->writeInt32(0);
//            buffer->writeInt32((int32_t)GetAccountId());
//            buffer->writeString(captcha);
//            buffer->writeString(address);
//            buffer->writeBytes(signBytes,sign.size());
        }else{
//            buffer = BuffersStorage::getInstance().getFreeBuffer(NativeInvokeHeaderSize + len );
//            buffer->writeInt32(NativeInvokeHeaderSize + len);
//            buffer->writeInt32(NativeInvokeType_PDU_RECV);
//            buffer->writeInt32(0);
//            buffer->writeInt32((int32_t)GetAccountId());
//            buffer->writeBytes(pPdu->GetBuffer(),len);
        }
//
//        buffer->position(0);
//        buffer->reuse();
//        MsgConnManager::onNotify(buffer);


        outBuf->writeInt32(NativeInvokeHeaderSize + pdu_len);
        outBuf->writeInt32(NativeInvokeType_PDU_RECV);
        outBuf->writeInt32(0);
        outBuf->writeInt32(1001);
        outBuf->writeBytes(buff->copy(pdu_len));
    }

}

void CMsgConn::HandlePdu(CImPdu* pPdu)
{
    if(pPdu->GetCommandId() == 1793){//heart beat
        return;
    }
    //DEBUG_D("HandlePdu GetCommandId: %d",pPdu->GetCommandId());
    auto len = pPdu->GetLength();
    //DEBUG_D("len: %d",len);

    NativeByteBuffer *buffer;
    if(pPdu->GetReversed() == 1 ){
        if(connectionState != ConnectionLogged){
            connectionState = ConnectionLogged;
            MsgConnManager::onConnectionStateChanged(connectionState,GetAccountId());
        }

        unsigned char shared_secret[32];
        unsigned char iv[16];
        unsigned char aad[16];
        memcpy(shared_secret,hex_to_string(m_sharedKeyHex).data(),32);
        memcpy(iv,hex_to_string(m_ivHex).data(),16);
        memcpy(aad,hex_to_string(m_aadHex).data(),16);

        auto cipherData = pPdu->GetBodyData();
        auto cipherDataLen = pPdu->GetBodyLength();

        unsigned char decData[1024];
        int decLen = aes_gcm_decrypt(
                cipherData, (int)cipherDataLen,
                shared_secret, iv,
                aad,
                decData);
        //DEBUG_D("decLen: %d",decLen);
        //DEBUG_D("decData: %s",ptp_toolbox::data::bytes_to_hex(decData,decLen).c_str());

        if (decLen > 0) {
            CImPdu pdu;
            pdu.SetPBMsg(decData,decLen);
            pdu.SetServiceId(pPdu->GetServiceId());
            pdu.SetCommandId(pPdu->GetCommandId());
            pdu.SetSeqNum(pPdu->GetSeqNum());
            pdu.SetReversed(0);
            pPdu = CImPdu::ReadPdu(pdu.GetBuffer(), pdu.GetLength());
            memset(decData, 0, sizeof(decData));
            len = pPdu->GetLength();
            //DEBUG_D("len: %d",len);
            //DEBUG_D("GetBodyLength: %d",pPdu->GetBodyLength());
            //DEBUG_D("GetBodyData: %s", bytes_to_hex_string(pPdu->GetBodyData(),pPdu->GetBodyLength()).c_str());
        }else{
            DEBUG_E("error decrypt body,cid:%d",pPdu->GetCommandId() );
            return;
        }
    }

    if(pPdu->GetCommandId() == 262){//auth captcha
        auto buf = pPdu->GetBodyData();
        unsigned char captchaBuff[6] = { 0 };
        memcpy(captchaBuff,buf + 2,6);

        unsigned char addressBuff[20] = { 0 };
        memcpy(addressBuff,buf + 2 + 6 + 2,20);
        //DEBUG_D("address: %s", ptp_toolbox::data::bytes_to_hex(addressBuff,20).c_str());

        unsigned char ivBuf[16] = { 0 };
        memcpy(ivBuf,buf + 2 + 6 + 2 + 20 + 2, 16);
        m_ivHex = ptp_toolbox::data::bytes_to_hex(ivBuf,16);
        //DEBUG_D("iv: %s", ptp_toolbox::data::bytes_to_hex(ivBuf,16).c_str());

        unsigned char aadBuff[16] = { 0 };

        memcpy(aadBuff,buf + 2 + 6 + 2 + 20 + 2 + 16 + 2, 16);
        m_aadHex = ptp_toolbox::data::bytes_to_hex(aadBuff,16);
        //DEBUG_D("aad: %s", ptp_toolbox::data::bytes_to_hex(aadBuff,16).c_str());

        unsigned char sign65Buf[65] = { 0 };
        memcpy(sign65Buf,buf + 2 + 6 + 2 + 20 + 2 + 16 + 2 + 16 + 2, 65);
        //DEBUG_D("signOut65: %s", ptp_toolbox::data::bytes_to_hex(sign65Buf,65).c_str());

        std::string captcha((char*) captchaBuff,6);
        //DEBUG_D("captcha: %s", captcha.c_str());

        string srv_address_hex;
        unsigned char pub_key_33[33] = {0};
        string msg_data = format_eth_msg_data(captcha);
        auto ret = recover_pub_key_from_sig_65(sign65Buf, msg_data, pub_key_33,srv_address_hex);
        if(!ret){
            DEBUG_E("recover_pub_key_from_sig_65 error!");
            return;
        }

        DEBUG_D("srv_address_hex: %s", srv_address_hex.c_str());

        if(srv_address_hex.substr(2) != ptp_toolbox::data::bytes_to_hex(addressBuff,20)){
            DEBUG_E("srv_address_hex error!");
            return;
        }

        unsigned char shared_secret[32];
        secp256k1_pubkey pub_key_raw;

        secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
        ret = secp256k1_ec_pubkey_parse(ctx,&pub_key_raw,pub_key_33,33);
        secp256k1_context_destroy(ctx);
        if(!ret){
            DEBUG_E("parse pub_key_raw error");
            return;
        }
        uint32_t accountId = GetAccountId();
        AccountManager::getInstance(accountId).upsertEntropy();
        AccountManager::getInstance(accountId).createShareKey(&pub_key_raw,shared_secret);

        //DEBUG_D("shared_secret: %s", ptp_toolbox::data::bytes_to_hex(shared_secret,32).c_str());
        m_sharedKeyHex = ptp_toolbox::data::bytes_to_hex(shared_secret,32);
        string address = AccountManager::getInstance(accountId).getAccountAddress();
        DEBUG_D("address: %s",address.c_str());
        string sign = AccountManager::getInstance(accountId).signMessage(captcha);
        auto *sizeCalculator = new NativeByteBuffer(true);
        sizeCalculator->clearCapacity();
        sizeCalculator->writeInt32(NativeInvokeHeaderSize);
        sizeCalculator->writeInt32(NativeInvokeType_SIGN_AUTH_CAPTCHA);
        sizeCalculator->writeInt32(0);
        sizeCalculator->writeInt32((int32_t)GetAccountId());
        sizeCalculator->writeString(captcha);
        sizeCalculator->writeString(address);
        auto* signBytes = (uchar_t *) sign.data();
        sizeCalculator->writeBytes(signBytes,sign.size());
        uint32_t size = sizeCalculator->capacity();
        buffer = BuffersStorage::getInstance().getFreeBuffer(size);
        buffer->writeInt32(size);
        buffer->writeInt32(NativeInvokeType_SIGN_AUTH_CAPTCHA);
        buffer->writeInt32(0);
        buffer->writeInt32((int32_t)GetAccountId());
        buffer->writeString(captcha);
        buffer->writeString(address);
        buffer->writeBytes(signBytes,sign.size());
    }else{
        buffer = BuffersStorage::getInstance().getFreeBuffer(NativeInvokeHeaderSize + len );
        buffer->writeInt32(NativeInvokeHeaderSize + len);
        buffer->writeInt32(NativeInvokeType_PDU_RECV);
        buffer->writeInt32(0);
        buffer->writeInt32((int32_t)GetAccountId());
        buffer->writeBytes(pPdu->GetBuffer(),len);
    }

    buffer->position(0);
    buffer->reuse();
    MsgConnManager::onNotify(buffer);
}



