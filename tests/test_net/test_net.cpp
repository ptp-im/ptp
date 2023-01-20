#include <gtest/gtest.h>

#include "ptp_global/Logger.h"
#include "ptp_net/AccountManager.h"
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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}