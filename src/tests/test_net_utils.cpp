#include <gtest/gtest.h>

#include "ptp_global/Logger.h"
#include "ptp_net/AccountManager.h"
#include "ptp_net/ClientConn.h"
//#include "ptp_business/CachePool.h"


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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}