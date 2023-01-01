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
#else
#include "ptp_common/Logger.h"
#include "ptp_net/Defines.h"
#endif


using namespace std;

class AccountManager {

public:
    AccountManager(uint32_t accountId);
    ~AccountManager();

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
    void sendPdu(uint8_t *pduBytes,uint32_t size);
    static void setCurrentAccountId(uint32_t accountId);
    static uint32_t getCurrentAccountId();
    static string m_configPath;
private:
    uint32_t m_accountId;
    string m_entropy;
    string m_address;
};

typedef unordered_map<uint32_t , AccountManager> AccountsMap_t;

#endif
