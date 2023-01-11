#include <gtest/gtest.h>

#include "ptp_global/Logger.h"
#include "ptp_protobuf/ImPdu.h"
#include "ptp_protobuf/PTP.Auth.pb.h"
#include "ptp_server/MsgSrvConn.h"
#include "ptp_crypto/secp256k1_helpers.h"
#include "ptp_server/CachePool.h"
#include "ptp_net/AccountManager.h"
#include "ptp_net/ClientConn.h"

uint32_t accountId              = 1001;
#define CONFIG_PATH             "../../conf/bd_server.conf"


class Delegate : public ConnectiosManagerDelegate {
    void onConnectionStateChanged(ConnectionState state, int32_t instanceNum) {
        DEBUG_D("onConnectionStateChanged,%d",state);
        if(state == ConnectionStateConnected){
            PTP::Auth::AuthCaptchaReq msg;
            ImPdu pdu;
            pdu.SetPBMsg(
                    &msg,
                    CID_AuthCaptchaReq,0);
            auto pMsgConn = get_conn(accountId);
            pMsgConn->SendPdu(&pdu);
        }
    }
    void onNotify(NativeByteBuffer *buffer,int32_t instanceNum){
        DEBUG_D("onNotify,%d");
    }
    void onUpdate(int32_t instanceNum) {}
    void onSessionCreated(int32_t instanceNum) {}
    void onUnparsedMessageReceived(int64_t reqMessageId, NativeByteBuffer *buffer, ConnectionType connectionType, int32_t instanceNum) {}
    void onLogout(int32_t instanceNum) {}
    void onUpdateConfig(TL_config *config, int32_t instanceNum) {}
    void onInternalPushReceived(int32_t instanceNum) {}
    void onBytesReceived(int32_t amount, int32_t networkType, int32_t instanceNum) {}
    void onBytesSent(int32_t amount, int32_t networkType, int32_t instanceNum) {}
    void onRequestNewServerIpAndPort(int32_t second, int32_t instanceNum) {}
    void onProxyError(int32_t instanceNum) {}
    void getHostByName(std::string domain, int32_t instanceNum, ConnectionSocket *socket) {}
    int32_t getInitFlags(int32_t instanceNum) {return 0;}
};

TEST(ptp_server_msg, init_msg_conn) {
    set_current_account_id(accountId);
    set_delegate(new Delegate());
    init_msg_conn(accountId);
}

TEST(ptp_server_msg, Auth) {
    CacheManager::setConfigPath(CONFIG_PATH);
    auto *pMsgSrvConn = new CMsgSrvConn();
    pMsgSrvConn->SetTest(true);
    pMsgSrvConn->SetHandle(100112);
    addMsgSrvConnByHandle(100112,pMsgSrvConn);
    PTP::Auth::AuthCaptchaReq msg;
    ImPdu pdu;
    pdu.SetPBMsg(&msg,CID_AuthCaptchaReq,0);
    pMsgSrvConn->HandlePdu(&pdu);
    auto pPdu = pMsgSrvConn->ReadTestPdu();
    ASSERT_EQ(pPdu->GetCommandId(),CID_AuthCaptchaRes);
    PTP::Auth::AuthCaptchaRes msg_rsp;
    auto res = msg_rsp.ParseFromArray(pPdu->GetBodyData(), (int)pPdu->GetBodyLength());
    ASSERT_EQ(res,true);
    DEBUG_D("captcha:%s",msg_rsp.captcha().c_str());
    DEBUG_D("server address:%s", bytes_to_hex_string(reinterpret_cast<const uint8_t *>(msg_rsp.address().c_str()), msg_rsp.address().size()).c_str());
    DEBUG_D("iv:%s", bytes_to_hex_string(reinterpret_cast<const uint8_t *>(msg_rsp.iv().c_str()), 16).c_str());
    DEBUG_D("aad:%s", bytes_to_hex_string(reinterpret_cast<const uint8_t *>(msg_rsp.aad().c_str()), 16).c_str());
    DEBUG_D("sign:%s", bytes_to_hex_string(reinterpret_cast<const uint8_t *>(msg_rsp.sign().c_str()), 65).c_str());
    ASSERT_EQ(msg_rsp.captcha().empty(), false);
    ASSERT_EQ(msg_rsp.address().empty(), false);
    ASSERT_EQ(msg_rsp.iv().empty(), false);
    ASSERT_EQ(msg_rsp.aad().empty(), false);
    ASSERT_EQ(msg_rsp.sign().empty(), false);

    string msg_data = AccountManager::format_sign_msg_data(msg_rsp.captcha());
    DEBUG_D("msg_data: %s", msg_data.c_str());
    unsigned char pub_key_33[33] = {0};
    string srv_address_hex;
    bool ret = recover_pub_key_and_address_from_sig((unsigned char *) msg_rsp.sign().c_str(), msg_data, pub_key_33, srv_address_hex);
    if(!ret){
        DEBUG_E("recover_pub_key_from_sig_65 error!");
        return;
    }

    DEBUG_D("server address rec: %s", srv_address_hex.c_str());

    unsigned char shared_secret[32];
    secp256k1_pubkey pub_key_raw;

    secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
    ret = secp256k1_ec_pubkey_parse(ctx,&pub_key_raw,pub_key_33,33);
    secp256k1_context_destroy(ctx);
    if(!ret){
        DEBUG_E("parse pub_key_raw error");
        return;
    }
    DEBUG_D("server pubKey: %s", bytes_to_hex_string(pub_key_33,33).c_str());
    int accountId = 1001;
    string captcha = msg_rsp.captcha();
    string accountAddress = AccountManager::getInstance(accountId).getAccountAddress();
    AccountManager::getInstance(accountId).createShareKey(&pub_key_raw,shared_secret);
    DEBUG_D("shared_secret: %s", ptp_toolbox::data::bytes_to_hex(shared_secret,32).c_str());
    DEBUG_D("client address: %s", accountAddress.c_str());
    PTP::Auth::AuthLoginReq msg_login;
    msg_login.set_client_type(PTP::Common::CLIENT_TYPE_ANDROID);
    msg_login.set_client_version("macos/10.1");
    msg_login.set_captcha(msg_rsp.captcha());
    msg_login.set_address(accountAddress);
    unsigned char sign65Buf[65] = { 0 };
    AccountManager::getInstance(accountId).signMessage(captcha,sign65Buf);
    msg_login.set_sign(sign65Buf,65);
    ImPdu pdu_login;
    pdu_login.SetPBMsg(&msg_login,CID_AuthLoginReq,0);
    pMsgSrvConn->HandlePdu(&pdu_login);
    auto pPdu1 = pMsgSrvConn->ReadTestPdu();
    ASSERT_EQ(pPdu1->GetCommandId(),CID_AuthLoginRes);
    PTP::Auth::AuthLoginRes msg_login_rsp;
    res = msg_login_rsp.ParseFromArray(pPdu1->GetBodyData(), (int)pPdu1->GetBodyLength());
    ASSERT_EQ(res,true);
    DEBUG_D("uid: %d", msg_login_rsp.user_info().uid());
    DEBUG_D("address: %s", msg_login_rsp.user_info().address().c_str());

}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
