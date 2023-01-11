#ifndef AccountManager_H
#define AccountManager_H
#include <unordered_map>
#include <pthread.h>
#include <queue>
#include <string>
#include <functional>
#include <map>
#include <atomic>
#include "Defines.h"
#include "Config.h"
#include "BuffersStorage.h"
#include "NativeByteBuffer.h"
#include "secp256k1/secp256k1.h"
#include "ptp_protobuf/ImPdu.h"
#include "ptp_global/UtilPdu.h"
#include "ptp_wallet/MnemonicHelper.h"
#include "ptp_wallet/HDKey.h"
#include "ptp_crypto/keccak.h"
#include "ptp_crypto/secp256k1_helpers.h"
#include "ptp_crypto/crypto_helpers.h"
#include "ptp_crypto/aes_encryption.h"
#include "ptp_crypto/AesSimple.h"


#define NativeInvokeHeaderSize      16
#define NativeInvokeBoolSize        4
#define NativeInvokeIntSize         4
#define NativeInvokeDefaultSepNum   0
#define SOCKET_BUF_SIZE             (100 * 1024 * 1024)

//#define MSG_SERVER_IP_1             "192.168.43.244"
#define MSG_SERVER_IP_1             "127.0.0.1"
#define MSG_SERVER_PORT_1           7881
#define MSFS_SERVER_IP_1            "192.168.43.244"
#define MSFS_SERVER_PORT_1          7841

enum NativeInvokeType {
    NativeInvokeType_HEART_BEAT = 1000,
    NativeInvokeType_SET_WORDS = 1001,
    NativeInvokeType_GET_WORDS = 1002,
    NativeInvokeType_PDU_RECV = 1003,
    NativeInvokeType_SIGN_AUTH_CAPTCHA = 1004,
    NativeInvokeType_GEN_WORDS = 1005,
    NativeInvokeType_PDU_SEND = 1006,
    NativeInvokeType_PDU_UPLOAD = 100601,
    NativeInvokeType_PDU_DOWNLOAD = 100602,
    NativeInvokeType_STATE_CHANGE = 1007,
    NativeInvokeType_SIGN_GROUP_MSG = 1008,
    NativeInvokeType_SWITCH_ACCOUNT = 1009,
    NativeInvokeType_CLOSE_SOCKET = 1010,
};

using namespace std;

class Connection;
class AccountManager {

public:
    AccountManager(uint32_t accountId);
    ~AccountManager();

    bool setWords(const std::string& words);
    uint32_t GetAccountId(){return m_accountId;};
    string GetEntropy(){return m_entropy;};
    void upsertEntropy();
    string getEntropyStorePathName();
    bool createShareKey(secp256k1_pubkey *pub_key,unsigned char *shared_key);
    void signMessage(const string& message,unsigned char *signOut65);
    void signGroupMessage(const string& message,int32_t groupIdx,unsigned char *signOut65);

    static void setConfigPath(string& configPath);
    static void setDelegate(ConnectiosManagerDelegate* delegate);
    static void onConnecting(int32_t instanceNum,int socketFd,ConnectionSocket *connection);
    static void onConnect(int32_t instanceNum,int socketFd);
    static void onClose(int32_t instanceNum,int socketFd,int32_t reason, int32_t error);
    static void onRead(int32_t instanceNum,int socketFd,NativeByteBuffer *buff,uint32_t size);
    static AccountManager &getInstance(uint32_t accountId);
    string getWords();
    static bool isAddressExists(const std::string& address);
    static string getAddressByWords(const std::string& words);
    static string genWords();
    static bool validateWords(const std::string& words);
    string getAccountAddress();
    static string getAddressByEntropy(const string& entropy);
    string signMessage(const string& message);
    string signGroupMessage(const string& message,int32_t groupIdx);
    static string verifyMessageRecoverAddress(const unsigned char *sig_65, const string& message);
    static void setCurrentAccountId(uint32_t accountId);
    static uint32_t getCurrentAccountId();
    static AccountManager getCurrentAccount();
    static void onNotify(NativeByteBuffer *buffer,int32_t instanceNum);
    static void onConnectionStateChanged(ConnectionState state, uint32_t accountId);
    static void handleInvoke(NativeByteBuffer *task,uint32_t instanceNum);
    static string format_sign_msg_data(const string &message,SignMsgType signMsgType = SignMsgType_ptp);
    void handlePdu(NativeByteBuffer * buff,uint32_t size,uint32_t instanceNum);
    void handlePdu(CImPdu *pPdu);
    void SetInstanceNum(int32_t instanceNum);
    int32_t GetInstanceNum(){return m_instanceNum;};
    void onHeartBeat();
    int SendPduBuf(uint8_t* pduBytes,uint32_t size);
    int SendPdu(CImPdu* pPdu);
    int SendBuf(uint8_t* buf,uint32_t size);
    int _SendPdu(CImPdu* pPdu);
    void setConnectionState(ConnectionState state);
    ConnectionState getConnectionState();
    void closeSocket();
    bool isConnected();
    bool isClosed();
    bool isConnecting();
private:
    ConnectionState m_connectionState;
    int32_t         m_instanceNum;
    uint32_t        m_accountId;
    string          m_entropy;
    string          m_address;
    string		    m_ivHex;
    string		    m_aadHex;
    string		    m_sharedKeyHex;
};

typedef unordered_map<uint32_t , AccountManager> AccountsMap_t;
typedef unordered_map<int, AccountManager> SocketAccountMap_t;

#endif
