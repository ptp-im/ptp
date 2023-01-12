#include <gtest/gtest.h>

#include "ptp_global/Logger.h"
#include "ptp_net/AccountManager.h"
#include "ptp_net/ClientConn.h"

#include "ptp_global/Logger.h"
#include "ptp_net/AccountManager.h"
#include "ptp_net/ClientConn.h"
//#include "ptp_business/CachePool.h"

#include "ptp_protobuf/PTP.Auth.pb.h"
using namespace PTP::Common;

TEST(ptp_net, AccountManager) {
    uint32_t accountId = 1001;
    AccountManager::getInstance(accountId).upsertEntropy();
    string address = AccountManager::getInstance(accountId).getAccountAddress();
    DEBUG_D("address:%s",address.c_str());
    ASSERT_TRUE(!address.empty() );
}

TEST(pt_net, Buffer1) {
    uint16_t tt = 1;
    auto *sizeCalculator = new NativeByteBuffer(true);
    sizeCalculator->clearCapacity();
    sizeCalculator->writeInt32(1);
    sizeCalculator->writeInt32((uint32_t) tt);
    auto len = sizeCalculator->capacity();
    NativeByteBuffer *buffer1 = BuffersStorage::getInstance().getFreeBuffer(len);
    buffer1->writeInt32(len);
    buffer1->writeInt32((uint32_t) tt);
    buffer1->position(0);
//
//    pushBuffer(len,buffer1);
//    buffer1->position(0);
//    buffer1->writeInt32(len);
//    buffer1->writeInt32((uint32_t) 2);
//    pushBuffer(len,buffer1);
//    buffer1->position(0);
//    buffer1->writeInt32(len);
//    buffer1->writeInt32((uint32_t) 3);
//    pushBuffer(len,buffer1);
//    delete buffer1;
//    while (true){
//        if(listQueue.empty()){
//            break;
//        }
//        auto tttt = listQueue.front();
//        handleTask(tttt);
//        delete tttt;
//        listQueue.pop();
//    }

    ASSERT_TRUE(true);
}
//TEST(pt_net, Buffer) {
//    ConnectiosManagerDelegate *de = new Delegate();
//    uint8_t t[] = "test";
//    uint16_t tt = 12;
//    auto *sizeCalculator = new NativeByteBuffer(true);
//    sizeCalculator->clearCapacity();
//    sizeCalculator->writeInt32(1);
//    sizeCalculator->writeInt32((uint32_t) tt);
//    sizeCalculator->writeBytes(t,sizeof t);
//    auto l = sizeCalculator->capacity();
//    NativeByteBuffer *buffer1 = BuffersStorage::getInstance().getFreeBuffer(l);
//    buffer1->writeInt32(l);
//    buffer1->writeInt32((uint32_t) tt);
//    buffer1->writeBytes(t,sizeof t);
//    buffer1->position(0);
//    de->onUnparsedMessageReceived(0, buffer1, ConnectionTypeGeneric, 0);
//    buffer1->reuse();
//    ASSERT_TRUE(true);
//}
//
//
//TEST(pt_net, Wallet) {
//    uint32_t accountId = time(nullptr);
//    bool res;
//    res = MsgConnManager::getInstance(0).upsertEntropy(accountId);
//    ASSERT_TRUE(res);
//    clearAccountIdEntropyMap();
//    res = MsgConnManager::getInstance(0).upsertEntropy(accountId,"");
//    ASSERT_TRUE(!res);
//    string address = getAccountAddress(accountId);
//    ASSERT_TRUE(!address.empty());
//    res = MsgConnManager::getInstance(0).upsertEntropy(accountId,address);
//    ASSERT_TRUE(res);
//    string message = "test_msg";
//    string sign = MsgConnManager::getInstance(0).signMessage(accountId,message);
//    string address1 = MsgConnManager::getInstance(0).verifyMessageRecoverAddress(sign,message);
//    ASSERT_TRUE(address1 == address);
//}


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