#include <gtest/gtest.h>
#include "ptp_wallet/MnemonicHelper.h"
#include "ptp_wallet/HDKey.h"
#include "ptp_net/MsgConnManager.h"
#include "ptp_net/AccountManager.h"
#include "ptp_net/Defines.h"

#include "ptp_net/NativeByteBuffer.h"
#include "ptp_net/BuffersStorage.h"
#include "ptp_common/ImPduBase.h"


class Delegate : public ConnectiosManagerDelegate {

    void onUpdate(int32_t instanceNum) {

    }

    void onSessionCreated(int32_t instanceNum) {

    }

    void onConnectionStateChanged(ConnectionState state, int32_t accountId) {
        DEBUG_D("onConnectionStateChanged:%d,accountId:%d",state,accountId);
        if(state == ConnectionState::ConnectionStateConnected){
            MsgConnManager::close(accountId);
        }
    }

    void onUnparsedMessageReceived(int64_t reqMessageId, NativeByteBuffer *buffer, ConnectionType connectionType, int32_t instanceNum) {
        if (connectionType == ConnectionTypeGeneric) {
        }
    }

    void onNotify(NativeByteBuffer *buffer) {

    }

    void onLogout(int32_t instanceNum) {

    }

    void onUpdateConfig(TL_config *config, int32_t instanceNum) {


    }

    void onInternalPushReceived(int32_t instanceNum) {

    }

    void onBytesReceived(int32_t amount, int32_t networkType, int32_t instanceNum) {

    }

    void onBytesSent(int32_t amount, int32_t networkType, int32_t instanceNum) {

    }

    void onRequestNewServerIpAndPort(int32_t second, int32_t instanceNum) {

    }

    void onProxyError(int32_t instanceNum) {

    }

    void getHostByName(std::string domain, int32_t instanceNum, ConnectionSocket *socket) {

    }

    int32_t getInitFlags(int32_t instanceNum) {

    }
};

TEST(pt_net, MsgConn) {
    bool t = true;
    bool f = false;
    DEBUG_E("t:%d f:%d",t,f);
    MsgConnManager::setDelegate(new Delegate());
    auto m1 = AccountManager::getInstance(1);
    auto m2 = AccountManager::getInstance(2);
    auto m3 = AccountManager::getInstance(3);
    auto m4 = AccountManager::getInstance(4);
    auto m5 = AccountManager::getInstance(1);

    ASSERT_EQ(m1.GetAccountId(),1);
    ASSERT_EQ(m2.GetAccountId(),2);
    ASSERT_EQ(m3.GetAccountId(),3);
    ASSERT_EQ(m4.GetAccountId(),4);
    ASSERT_EQ(m5.GetAccountId(),1);
    uint32_t accountId = 10011;

    AccountManager::setCurrentAccountId(accountId);
    string p = "/tmp";
    AccountManager::setConfigPath(p);
    MsgConnManager::initMsgConn(AccountManager::getCurrentAccountId());

}


TEST(pt_net, Buffer) {
    ConnectiosManagerDelegate *de = new Delegate();
    uint8_t t[] = "test";
    uint16_t tt = 12;
    auto *sizeCalculator = new NativeByteBuffer(true);
    sizeCalculator->clearCapacity();
    sizeCalculator->writeInt32(1);
    sizeCalculator->writeInt32((uint32_t) tt);
    sizeCalculator->writeBytes(t,sizeof t);
    auto l = sizeCalculator->capacity();
    NativeByteBuffer *buffer1 = BuffersStorage::getInstance().getFreeBuffer(l);
    buffer1->writeInt32(l);
    buffer1->writeInt32((uint32_t) tt);
    buffer1->writeBytes(t,sizeof t);
    buffer1->position(0);
    de->onUnparsedMessageReceived(0, buffer1, ConnectionTypeGeneric, 0);
    buffer1->reuse();
    ASSERT_TRUE(true);
}


TEST(pt_net, authLoginReqBuf) {
    uint32_t accountId = 200121;
    AccountManager::getInstance(accountId).upsertEntropy();
    string captcha = "captcha";
    string address = AccountManager::getInstance(accountId).getAccountAddress();
    string sign = AccountManager::getInstance(accountId).signMessage(captcha);
    string address_ = AccountManager::verifyMessageRecoverAddress(
            reinterpret_cast<const unsigned char *>(sign.data()), captcha);

    uchar_t* signBytes = (uchar_t *) sign.data();
    uint32_t clientType = 18;
    auto *sizeCalculator = new NativeByteBuffer(true);
    sizeCalculator->clearCapacity();
    sizeCalculator->writeInt32(clientType);
    sizeCalculator->writeString(captcha);
    sizeCalculator->writeString(address);
    sizeCalculator->writeBytes(signBytes,sign.size());
    uint32_t bufLen = sizeCalculator->capacity();
    auto *buf = new NativeByteBuffer(bufLen);
    buf->writeInt32(clientType);
    buf->writeString(captcha);
    buf->writeString(address);
    buf->writeBytes(signBytes,sign.size());
    buf->position(0);
    auto* body = new uchar_t[bufLen];
    buf->readBytes(body,bufLen, nullptr);

    CImPdu pdu;
    pdu.SetPBMsg(body, bufLen);
    delete[] body;
    pdu.SetServiceId(1);
    pdu.SetCommandId(257);
    ASSERT_TRUE(pdu.GetLength() == 16 + bufLen);
    auto *buf1 = new NativeByteBuffer(pdu.GetLength());
    buf1->writeBytes(pdu.GetBuffer(),pdu.GetLength());
    buf1->position(16);
    uint32_t clientType1 = buf1->readInt32(nullptr);
    string captcha1 = buf1->readString(nullptr);
    string address1 = buf1->readString(nullptr);

    auto* signBytes1 = new uchar_t[65];
    buf1->readBytes(signBytes1,65,nullptr);

    ASSERT_TRUE(clientType1 == clientType);
    ASSERT_TRUE(captcha1 == captcha);
    ASSERT_TRUE(address1 == address);
    string address2 = AccountManager::verifyMessageRecoverAddress(signBytes1,captcha);
    ASSERT_TRUE(address2 == address);

}

TEST(pt_net, Wallet1) {
    const char* words = "inhale inner skate tobacco sketch elegant prepare fresh neglect turtle stable one";
    DEBUG_D("words: %s",words);
    auto res1 = PTPWallet::MnemonicHelper::validateWords("en", words);
    ASSERT_TRUE(res1);
    unsigned char seed_out[64];
    size_t seed_len;
    PTPWallet::MnemonicHelper::wordsToSeed(
            words,
            seed_out,
            &seed_len);
    ASSERT_EQ(seed_len,64);
    uint8_t entropyBytes[16];
    auto entropyBytesLen = PTPWallet::MnemonicHelper::mnemonicWordsToEntropy(words,"en",entropyBytes,BIP39_ENTROPY_LEN_128);
    ASSERT_EQ(entropyBytesLen,16);
    auto entropy = ptp_toolbox::data::bytes_to_hex(entropyBytes,16);

    auto address = AccountManager::getAddressByEntropy(entropy);

    PTPWallet::MnemonicHelper::MnemonicResult mnemonicResult21 = PTPWallet::MnemonicHelper::entropyHexToMnemonic(entropy, "en");
    ASSERT_EQ(words,mnemonicResult21.raw);

    PTPWallet::HDKey hdKey1 = PTPWallet::HDKeyEncoder::makeEthRootKey(mnemonicResult21.raw.data());
    PTPWallet::HDKeyEncoder::makeEthExtendedKey(hdKey1, "m/44'/60'/0'/0/0");
    DEBUG_D("rootHdKey1: %s",hdKey1.privateKey.to_hex().c_str());

    string address11 = PTPWallet::HDKeyEncoder::getEthAddress(hdKey1);
    DEBUG_D("address: %s",address.c_str());
    DEBUG_D("address11: %s",address11.c_str());

    ASSERT_EQ(address11,address);

}

TEST(pt_net, Wallet) {
    uint32_t accountId = time(nullptr);
    AccountManager::getInstance(accountId).upsertEntropy();
    string address = AccountManager::getInstance(accountId).getAccountAddress();
    ASSERT_TRUE(!address.empty());
    AccountManager::getInstance(accountId).upsertEntropy();
    string message = "test_msg";
    unsigned char signOut65[65];
    AccountManager::getInstance(accountId).signMessage(message,signOut65);
    string address1 = AccountManager::verifyMessageRecoverAddress(signOut65,message);
    ASSERT_TRUE(address1 == address);
}

TEST(ptp_wallet, GenerateMnemonics) {
    PTPWallet::MnemonicHelper::MnemonicResult m1 = PTPWallet::MnemonicHelper::generate();
    ASSERT_TRUE(PTPWallet::MnemonicHelper::validateWords("en", m1.raw.data()));
    std::cout << m1.raw << std::endl;
    string words = "dress diesel matter soccer dial tunnel bonus choice typical endless dragon grid";
    std::cout << words << std::endl;
    ASSERT_TRUE(PTPWallet::MnemonicHelper::validateWords("en", words.data()));

    string words1 = "dress diesel matter soccer dial tunnel bonus choice typical endless dragon gri1d";
    ASSERT_FALSE(PTPWallet::MnemonicHelper::validateWords("en", words1.data()));

    uint32_t accountId = time(nullptr);
    auto res = AccountManager::getInstance(accountId).setWords(words);
    ASSERT_TRUE(res);
    string words2 = AccountManager::getInstance(accountId).getWords();
    std::cout << words2 << std::endl;
    ASSERT_EQ(words,words2);
}

TEST(ptp_wallet, GenerateMnemonics1) {
    string words = AccountManager::genWords();
    uint32_t size = 16 + (uint32_t) words.length() + 2;

    auto *sizeCalculator = new NativeByteBuffer(true);
    sizeCalculator->clearCapacity();
    sizeCalculator->writeString(words);
    std::cout << words.length() << std::endl;
    std::cout << sizeCalculator->capacity() << std::endl;

    NativeByteBuffer *buffer = BuffersStorage::getInstance().getFreeBuffer(size);
    buffer->writeInt32((int32_t) size);
    buffer->writeInt32(1005);
    buffer->writeInt32(0);
    buffer->writeInt32(10001);
    buffer->writeString(words);
    ASSERT_EQ(1,1);
}

TEST(ptp_wallet, CheckAddress) {
    string words = AccountManager::genWords();
    auto address = AccountManager::getAddressByWords(words);
    auto res = AccountManager::isAddressExists(address);
    ASSERT_EQ(res, false);
    uint32_t accountId = time(nullptr);
    AccountManager::getInstance(accountId).setWords(words);
    res = AccountManager::isAddressExists(address);
    ASSERT_EQ(res, true);
}

TEST(ptp_wallet, AccountAddress) {
    uint32_t accountId = time(nullptr);
    auto account = AccountManager::getInstance(accountId);
    string address = account.getAccountAddress();
    string address1 = account.getAccountAddress();
    ASSERT_EQ(address1, address);
}
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
