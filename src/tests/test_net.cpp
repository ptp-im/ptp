#include <gtest/gtest.h>

#include "ptp_global/Logger.h"
#include "ptp_net/AccountManager.h"
#include "ptp_net/ClientConn.h"

#include "PTP.Auth.pb.h"
using namespace PTP::Common;

uint32_t accountId = 1001;

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
    int32_t getInitFlags(int32_t instanceNum) {return 1;}
};

TEST(ptp_net, MsgConnClient) {
    set_current_account_id(accountId);
    set_delegate(new Delegate());
    init_msg_conn(accountId);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}