#ifndef AccountManager_H
#define AccountManager_H
#include <unordered_map>
#include <pthread.h>
#include <queue>
#include <string>
#include <functional>
#include <map>
#include <atomic>
#include <secp256k1/secp256k1.h>

#ifdef ANDROID
#include <jni.h>
#include "../../tgnet/FileLog.h"
#include "../../tgnet/Defines.h"
#include "../../tgnet/NativeByteBuffer.h"
#else
#include "ptp_common/Logger.h"
#include "ptp_net/Defines.h"
#include "ptp_net/NativeByteBuffer.h"
#endif

using namespace std;

class AccountManager {

public:
    AccountManager(uint32_t accountId);
    ~AccountManager();
    static void setDelegate(ConnectiosManagerDelegate* delegate);
    static void onConnecting(int32_t instanceNum);
    static void onConnect(int32_t instanceNum,int socketFd);
    static void onClose(int32_t instanceNum,int socketFd,int32_t reason, int32_t error);
    static void onRead(int32_t instanceNum,int socketFd,NativeByteBuffer *buff,uint32_t size);
    static AccountManager findAccountBySocket(int socketFd);

    static AccountManager &getInstance(uint32_t accountId);
    string getWords();
    static bool isAddressExists(const std::string& address);
    static string getAddressByWords(const std::string& words);
    static string genWords();
    bool setWords(const std::string& words);
    static bool validateWords(const std::string& words);
    uint32_t GetAccountId(){return m_accountId;};
    string GetEntropy(){return m_entropy;};
    static void setConfigPath(string& configPath);
    void upsertEntropy();
    string getAccountAddress();
    static string getAddressByEntropy(const string& entropy);
    string getEntropyStorePathName();
    bool createShareKey(secp256k1_pubkey *pub_key,unsigned char *shared_key);
    string signMessage(const string& message);
    void signMessage(const string& message,unsigned char *signOut65);
    string signGroupMessage(const string& message,int32_t groupIdx);
    void signGroupMessage(const string& message,int32_t groupIdx,unsigned char *signOut65);
    static string verifyMessageRecoverAddress(const unsigned char *sig_65, const string& message);
    void handlePdu(NativeByteBuffer * buff,uint32_t size);
    static string m_configPath;

    static void setCurrentAccountId(uint32_t accountId);
    static uint32_t getCurrentAccountId();
    static AccountManager getCurrentAccount();
    static void onNotify(NativeByteBuffer *buffer);
    void SetInstanceNum(int32_t instanceNum);
    int32_t GetInstanceNum(int32_t instanceNum);
    NativeByteBuffer *onHeartBeat();
    static void onConnectionStateChanged(ConnectionState state, uint32_t accountId);
    NativeByteBuffer * getPduSendBuf(uint8_t* pduBytes,uint32_t size);
    void setConnectionState(ConnectionState state);
private:
    ConnectionState m_connectionState;
    int32_t m_instanceNum;
    uint32_t m_accountId;
    string m_entropy;
    string m_address;
    string		m_ivHex;
    string		m_aadHex;
    string		m_sharedKeyHex;
};

typedef unordered_map<uint32_t , AccountManager> AccountsMap_t;
typedef unordered_map<int, AccountManager> SocketAccountMap_t;


#endif
