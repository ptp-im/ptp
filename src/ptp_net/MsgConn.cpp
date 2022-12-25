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

static ConnectionState connectionState;

static ConnMap_t g_msg_conn_map;
static ServerInfoMap_t g_server_info_map;

static uint32_t		g_msg_conn_count = 1;
static uint32_t		current_account_id = 0;
static uint32_t		cnt = 0;
static uint32_t		connecting_cnt = 0;

static bool 		net_loop_stated = false;

static string		ivHex;
static string		aadHex;
static string		sharedKeyHex;


static void msg_conn_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
    NativeByteBuffer *buffer = BuffersStorage::getInstance().getFreeBuffer(NativeInvokeHeaderSize );
    buffer->writeInt32(NativeInvokeHeaderSize);
    buffer->writeInt32(NativeInvokeType_HEART_BEAT);
    buffer->writeInt32(0);
    buffer->writeInt32(0);
    buffer->position(0);
    buffer->reuse();
    MsgConnManager::onNotify(buffer);

    ConnMap_t::iterator it_old;
	CMsgConn* pConn = NULL;
	uint64_t cur_time = get_tick_count();
	for (auto it = g_msg_conn_map.begin(); it != g_msg_conn_map.end(); ) {
		it_old = it;
		it++;

		pConn = (CMsgConn*)it_old->second;
		if (pConn->IsOpen()) {
			pConn->OnTimer(cur_time);
		}
	}
    bool hasAccount = false;
	// reconnect msg Server
	// will reconnect in 4s, 8s, 16s, 32s, 4s 8s ...
    for (auto & it : g_server_info_map) {
        if(it.first == current_account_id){
            hasAccount = true;
            cnt++;
            if(cnt % 10 == 0){
                if(connectionState == ConnectionStateConnecting){
                    connecting_cnt += 1;
                    if(connecting_cnt > 2 && it.second[0].serv_conn){
                        connecting_cnt = 0;
                        auto pMsgConn = (CMsgConn *)it.second[0].serv_conn;
                        pMsgConn->Close();
                        continue;
                    }
                }else{
                    connecting_cnt = 0;
                }
                DEBUG_D("check,state:%u",connectionState);
            }
            serv_check_reconnect<CMsgConn>(it.second, g_msg_conn_count);
        }else{
            for (uint32_t i = 0; i < g_msg_conn_count; i++) {
                it.second[i].idle_cnt = 0;
                it.second[i].reconnect_cnt = 0;
            }
        }
    }
    if(!hasAccount){
        init_msg_conn(current_account_id);
    }
}


void close_msg_conn(uint32_t account_id){
    for (auto & it : g_server_info_map) {
        for (uint32_t i = 0; i < g_msg_conn_count; i++) {
            if(account_id == it.first){
                it.second[i].reconnect_enable = false;
                auto pMsgConn = (CMsgConn * )it.second[i].serv_conn;
                if(pMsgConn && pMsgConn->IsOpen()){
                    pMsgConn->Close();
                }
            }
        }
    }
    if(account_id == current_account_id){
        current_account_id = 0;
        netlib_stop_event();
        net_loop_stated = false;
    }
}

uint32_t get_current_account_id(){
    return current_account_id;
}

void set_current_account_id(uint32_t account_id){
    current_account_id = account_id;

}

void reset_msg_conn(){
    for (auto & it : g_server_info_map) {
        for (uint32_t i = 0; i < g_msg_conn_count; i++) {
            auto pMsgConn = it.second[i].serv_conn;
            if(pMsgConn){
                delete pMsgConn;
                it.second[i].serv_conn = NULL;
                DEBUG_D("reset_msg_conn account_id:%d",it.second[i].account_id);
            }
        }
    }
}
void init_msg_conn(uint32_t account_id)
{
    cnt = 0;
    set_current_account_id(account_id);
    bool hasAccount = false;
    for (auto & it : g_server_info_map) {
        for (uint32_t i = 0; i < g_msg_conn_count; i++) {
            if(account_id != it.first){
                it.second[i].reconnect_enable = true;
            }else{
                it.second[i].reconnect_enable = true;
                if(!hasAccount){
                    hasAccount = true;
                }
            }
        }
    }

    if(!hasAccount){
        auto server_list = new serv_info_t[g_msg_conn_count];
        string		server_ip = "192.168.43.244";
        uint16_t	server_port = 7881;
        for (uint32_t i = 0; i < g_msg_conn_count; i++) {
            server_list[i].server_ip = server_ip;
            server_list[i].server_port = server_port;
            server_list[i].reconnect_enable = true;
            server_list[i].idle_cnt = 0;
            server_list[i].account_id = account_id;
            server_list[i].reconnect_cnt = 0;
            server_list[i].serv_conn = NULL;
        }
        g_server_info_map.insert(make_pair(account_id, server_list));
    }

    DEBUG_D("init_msg_conn current_account_id:%d,hasAccount:%d,net_loop_stated:%d",account_id,hasAccount,net_loop_stated);

    if(!net_loop_stated){
        net_loop_stated = true;
        netlib_register_timer(msg_conn_timer_callback, NULL, 1000);
    }
}

CMsgConn* get_msg_conn(uint32_t account_id)
{
    uint32_t i = 0;
    CMsgConn* pMsgConn = NULL;
    for (auto it = g_server_info_map.begin(); it != g_server_info_map.end(); it++) {
        if(account_id == it->first){
            pMsgConn = (CMsgConn*)it->second[i].serv_conn;
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

void CMsgConn::Connect(const char* server_ip, uint16_t server_port, uint32_t serv_idx, uint32_t account_id)
{
    connectionState = ConnectionStateConnecting;
    MsgConnManager::onConnectionStateChanged(connectionState,account_id);
	m_serv_idx = serv_idx;
    m_accountId = account_id;
	m_handle = netlib_connect(server_ip, server_port, imconn_callback, (void*)&g_msg_conn_map);
    DEBUG_D("Connecting %s:%d aid:%d m_handle:%d", server_ip, server_port,account_id,m_handle);

	if (m_handle != NETLIB_INVALID_HANDLE) {
       //DEBUG_D("m_handle: %d,serv_idx: %d",m_handle,serv_idx);
       g_msg_conn_map.insert(make_pair(m_handle, this));
	}
}

void CMsgConn::Close()
{
    for (auto & it : g_server_info_map) {
        if(GetAccountId() == it.first){
            if(it.second[m_serv_idx].reconnect_enable){
                connectionState = ConnectionStateConnecting;
            }else{
                connectionState = ConnectionStateClosed;
                MsgConnManager::onConnectionStateChanged(ConnectionStateClosed,GetAccountId());
            }
            serv_reset<CMsgConn>(it.second, g_msg_conn_count, m_serv_idx);
            break;
        }
    }
	if (m_handle != NETLIB_INVALID_HANDLE) {
		netlib_close(m_handle);
		g_msg_conn_map.erase(m_handle);
	}
	ReleaseRef();
}

void CMsgConn::OnConfirm()
{
	DEBUG_D("Connected,aId:%d,caid:%d",GetAccountId(),current_account_id);
	m_bOpen = true;
    for (auto & it : g_server_info_map) {
        if(GetAccountId() == it.first){
            it.second[m_serv_idx].reconnect_cnt = 0;
            break;
        }
    }

    connectionState = ConnectionStateConnected;
    MsgConnManager::onConnectionStateChanged(ConnectionStateConnected,GetAccountId());
}

void CMsgConn::OnClose()
{
	DEBUG_D("Closed handle=%d,accountId:%d", m_handle,GetAccountId());
	Close();
}

void CMsgConn::OnTimer(uint64_t curr_tick)
{
    if (current_account_id != m_accountId) {
        DEBUG_D("OnTimer,current_account_id: %d,m_accountId: %d",current_account_id,m_accountId);
        Close();
    }else{
        if (curr_tick > m_last_send_tick + CLIENT_HEARTBEAT_INTERVAL && connectionState == ConnectionLogged) {
            CImPdu pdu;
            pdu.SetPBMsg(nullptr);
            pdu.SetServiceId(7);
            pdu.SetCommandId(1793);
            SendPdu(&pdu);
            DEBUG_D("heart beat... %d",CLIENT_HEARTBEAT_INTERVAL);
        }

        if (curr_tick > m_last_recv_tick + MOBILE_CLIENT_TIMEOUT) {
            DEBUG_D("conn to msg server timeout %d",MOBILE_CLIENT_TIMEOUT);
            Close();
        }
    }
}

int CMsgConn::SendPdu(CImPdu * pdu) {
    auto pPdu = CImPdu::ReadPdu(pdu->GetBuffer(), pdu->GetLength());
    if(pPdu->GetReversed() == 1 && pPdu->GetBodyLength() > 0){
        unsigned char shared_secret[32];
        unsigned char iv[16];
        unsigned char aad[16];
        memcpy(shared_secret,hex_to_string(sharedKeyHex).data(),32);
        memcpy(iv,hex_to_string(ivHex).data(),16);
        memcpy(aad,hex_to_string(aadHex).data(),16);

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
        return Send(pdu1.GetBuffer(), (int)pdu1.GetLength());
    }else{
        return Send(pPdu->GetBuffer(), (int)pPdu->GetLength());
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
        memcpy(shared_secret,hex_to_string(sharedKeyHex).data(),32);
        memcpy(iv,hex_to_string(ivHex).data(),16);
        memcpy(aad,hex_to_string(aadHex).data(),16);

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
        ivHex = ptp_toolbox::data::bytes_to_hex(ivBuf,16);
        //DEBUG_D("iv: %s", ptp_toolbox::data::bytes_to_hex(ivBuf,16).c_str());

        unsigned char aadBuff[16] = { 0 };

        memcpy(aadBuff,buf + 2 + 6 + 2 + 20 + 2 + 16 + 2, 16);
        aadHex = ptp_toolbox::data::bytes_to_hex(aadBuff,16);
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
        sharedKeyHex = ptp_toolbox::data::bytes_to_hex(shared_secret,32);
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


