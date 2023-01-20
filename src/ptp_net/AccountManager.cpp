#include "AccountManager.h"
#include "ClientConn.h"
#include "ptp_global/SocketClient.h"
#include "ptp_global/Helpers.h"
#include "ptp_crypto/md5.h"
#include "ptp_protobuf/PTP.Auth.pb.h"
#include "ptp_protobuf/ImPdu.h"

using namespace PTP::Common;

static uint32_t		                m_current_account_id = 0;
static AccountsMap_t                g_accounts_map;
static SocketAccountMap_t           g_socket_account_map;
static string                       g_configPath;
static ConnectiosManagerDelegate*   m_delegate;
static Connection*                  m_connection;

static uint32_t getStringSize(const string &str){
    auto *sizeCalculator = new NativeByteBuffer(true);
    sizeCalculator->clearCapacity();
    sizeCalculator->writeString(str);
    return sizeCalculator->capacity();
}

static string getConfigPath(){
    if (!g_configPath.empty() && g_configPath.find_last_of('/') != g_configPath.size() - 1) {
        g_configPath += "/";
    }
    return g_configPath;
};

static string getAccountAddressFile(const string &address){
    return getConfigPath() + address + ".dat";
};

static uint32_t encryptDataUseAecSimple(const string& plainData,const string& key,char** cipherData) {
    CAes cAes = CAes(key);
    uint32_t nOutLen;
    if(cAes.Encrypt(plainData.data(), plainData.size(), cipherData, nOutLen) == 0)
    {
        DEBUG_D("encryptDataUseAecSimple OK");
        return nOutLen;
    }else{
        DEBUG_E("encryptDataUseAecSimple error");
        return 0;
    }
}

static string getEncryptEntropyKey(uint32_t accountId, const std::string& address) {
    string enKey = to_string(accountId)+address;
    string enKey_hash;
    CMd5::MD5_Calculate(enKey.data(),enKey.size(),enKey_hash);
    return enKey_hash;
}

static uint32_t decryptDataUseAecSimple(const string& cipherData, const string& key,char** plainData) {
    CAes cAes = CAes(key);
    uint32_t nOutLen;

    if(cAes.Decrypt(cipherData.data(), cipherData.size(), plainData, nOutLen) == 0)
    {
        return nOutLen;
    }else{
        DEBUG_E("decryptDataUseAecSimple error");
        return 0;
    }
}

void AccountManager::setCurrentAccountId(uint32_t accountId){
    DEBUG_D("setCurrentAccountId accountId:%d,current_account_id:%d",accountId,m_current_account_id);

    if(accountId != m_current_account_id && m_current_account_id != 0){
        AccountManager::getInstance(m_current_account_id).closeSocket();
    }
    if(accountId != m_current_account_id){
        AccountManager::getInstance(accountId).SetInstanceNum(0);
    }
    m_current_account_id = accountId;
}

uint32_t AccountManager::getCurrentAccountId(){
    return m_current_account_id;
}

AccountManager::AccountManager(uint32_t accountId) {
    m_accountId = accountId;
    m_connectionState = ConnectionStateClosed;
}

void AccountManager::setDelegate(ConnectiosManagerDelegate* delegate)
{
    m_delegate = delegate;
}

AccountManager::~AccountManager() = default;

AccountManager& AccountManager::getInstance(uint32_t accountId) {
    for (auto & it : g_accounts_map) {
        if(accountId == it.first){
            return it.second;
        }
    }
    AccountManager instance(accountId);
    g_accounts_map.insert(make_pair(accountId, instance));
    for (auto & it : g_accounts_map) {
        if(accountId == it.first){
            return it.second;
            break;
        }
    }
}

AccountManager AccountManager::getCurrentAccount(){
    return AccountManager::getInstance(AccountManager::getCurrentAccountId());
}

void AccountManager::onConnecting(int32_t instanceNum,int socketFd,ConnectionSocket *connection) {
    m_connection = (Connection *)connection;
    DEBUG_D("onConnecting instanceNum=%d",instanceNum);
    if(instanceNum != 0 || socketFd == -1){
        return;
    }

    for (auto it = g_socket_account_map.begin(); it != g_socket_account_map.end(); ) {
        if(it->second.GetAccountId() == AccountManager::getCurrentAccountId()){
            g_socket_account_map.erase(it->first);
        }
    }
    g_socket_account_map.insert(make_pair(socketFd,AccountManager::getCurrentAccount()));
    AccountManager::getCurrentAccount().upsertEntropy();
    AccountManager::onConnectionStateChanged(ConnectionStateConnecting,AccountManager::getCurrentAccountId());
}

void AccountManager::onConnect(int32_t instanceNum,int socketFd){
    DEBUG_D("onConnect instanceNum=%d,socketFd=%d",instanceNum,socketFd);
    if(instanceNum != 0 || socketFd == -1){
        return;
    }
    AccountManager::onConnectionStateChanged(ConnectionStateConnected,AccountManager::getCurrentAccountId());
}

void AccountManager::onClose(int32_t instanceNum,int socketFd,int32_t reason, int32_t error){
    DEBUG_D("onClose instanceNum=%d,socketFd=%d,reason=%d,error=%d",instanceNum,socketFd,reason,error);
    if(instanceNum != 0){
        return;
    }

    if(socketFd != -1){
        for (auto it = g_socket_account_map.begin(); it != g_socket_account_map.end(); ) {
            if(it->first == socketFd){
                AccountManager::onConnectionStateChanged(ConnectionStateClosed,it->second.GetAccountId());
                g_socket_account_map.erase(socketFd);
                break;
            }
        }
    }
}

void AccountManager::onRead(int32_t instanceNum,int socketFd,NativeByteBuffer * buff,uint32_t size){
    DEBUG_D("onRead instanceNum=%d,socketFd=%d,size=%d",instanceNum,socketFd,size);
    if(instanceNum != 0){
        return;
    }
    if(socketFd != -1){
        for (auto it = g_socket_account_map.begin(); it != g_socket_account_map.end(); ) {
            if(it->first == socketFd){
                //it->second.handlePdu(buff,size,instanceNum);
                break;
            }
        }
    }
}


void AccountManager::handlePdu(CImPdu *pPdu){
    ImPdu *pPdu1 = (ImPdu *)pPdu;
    pPdu1->Dump();
    DEBUG_E("handlePdu,cid:%s,encrypt:%d",getActionCommandsName((ActionCommands)pPdu->GetCommandId()).c_str(),pPdu->GetReversed());
    if(pPdu->GetReversed() == 1 ){
        if(m_connectionState != ConnectionLogged){
            m_connectionState = ConnectionLogged;
            AccountManager::onConnectionStateChanged(m_connectionState,GetAccountId());
        }

        unsigned char shared_secret[32];
        unsigned char iv[16];
        unsigned char aad[16];
        memcpy(shared_secret,hex_to_string(m_sharedKeyHex.substr(2)).data(),32);
        memcpy(iv,hex_to_string(m_ivHex.substr(2)).data(),16);
        memcpy(aad,hex_to_string(m_aadHex.substr(2)).data(),16);

        DEBUG_D("shared_secret: %s",m_sharedKeyHex.c_str());
        DEBUG_D("iv: %s",m_ivHex.c_str());
        DEBUG_D("m_aadHex: %s",m_aadHex.c_str());
        auto cipherData = pPdu->GetBodyData();
        auto cipherDataLen = pPdu->GetBodyLength();
        DEBUG_D("pdu:size:%d, %s",cipherDataLen,ptp_toolbox::data::bytes_to_hex(cipherData,cipherDataLen).c_str());
        unsigned char decData[1024];
        int decLen = aes_gcm_decrypt(
                cipherData, (int)cipherDataLen,
                shared_secret, iv,
                aad,
                decData);
        DEBUG_D("decLen: %d",decLen);
        DEBUG_D("decData: %s",ptp_toolbox::data::bytes_to_hex(decData,decLen).c_str());

        if (decLen > 0) {
            ImPdu pdu;
            pPdu->SetPBMsg(decData,decLen);
            pPdu1 = (ImPdu *)pPdu;
            pPdu1->Dump();
            memset(decData, 0, sizeof(decData));
        }else{
            DEBUG_E("error decrypt body,cid:%s", getActionCommandsName((ActionCommands)pPdu->GetCommandId()).c_str());
            return;
        }
    }

    if(pPdu->GetCommandId() == CID_AuthCaptchaRes){//auth captcha
        string address = getAccountAddress();
        auto buf = pPdu->GetBodyData();
        unsigned char captchaBuff[7] = { 0 };
        memcpy(captchaBuff,buf + 2,6);
        captchaBuff[6] = '\0';
        unsigned char addressBuff[20] = { 0 };
        memcpy(addressBuff,buf + 2 + 6 + 2,20);

        unsigned char ivBuf[16] = { 0 };
        memcpy(ivBuf,buf + 2 + 6 + 2 + 20 + 2, 16);
        m_ivHex = ptp_toolbox::data::bytes_to_hex(ivBuf,16);
        DEBUG_D("iv: %s", ptp_toolbox::data::bytes_to_hex(ivBuf,16).c_str());

        unsigned char aadBuff[16] = { 0 };

        memcpy(aadBuff,buf + 2 + 6 + 2 + 20 + 2 + 16 + 2, 16);
        m_aadHex = ptp_toolbox::data::bytes_to_hex(aadBuff,16);
        DEBUG_D("aad: %s", ptp_toolbox::data::bytes_to_hex(aadBuff,16).c_str());

        unsigned char sign65Buf[65] = { 0 };
        memcpy(sign65Buf,buf + 2 + 6 + 2 + 20 + 2 + 16 + 2 + 16 + 2, 65);

        string captcha;
        captcha.append((char *)captchaBuff);
        string srv_address_hex;
        unsigned char pub_key_33[33] = {0};
        string msg_data = AccountManager::format_sign_msg_data(captcha);
        DEBUG_D("msg_data: %s", msg_data.c_str());
        DEBUG_D("server sign65Buf: %s", ptp_toolbox::data::bytes_to_hex(sign65Buf,65).c_str());
        auto ret = recover_pub_key_and_address_from_sig(sign65Buf, msg_data, pub_key_33,srv_address_hex);
        DEBUG_D("server address: %s,captcha:%s", ptp_toolbox::data::bytes_to_hex(addressBuff,20).c_str(),captchaBuff);
        DEBUG_D("server address rec: %s", srv_address_hex.c_str());
        if(!ret){
            DEBUG_E("recover_pub_key_from_sig_65 error!");
            return;
        }

        if(srv_address_hex != ptp_toolbox::data::bytes_to_hex(addressBuff,20)){
            DEBUG_E("srv_address_hex error!");
            closeSocket();
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
        createShareKey(&pub_key_raw,shared_secret);
        DEBUG_D("server pubKey: %s", bytes_to_hex_string(pub_key_33,33).c_str());
        DEBUG_D("shared_secret: %s", ptp_toolbox::data::bytes_to_hex(shared_secret,32).c_str());
        m_sharedKeyHex = ptp_toolbox::data::bytes_to_hex(shared_secret,32);
        unsigned char sign65Buf1[65] = { 0 };
        signMessage(captcha,sign65Buf1);
        PTP::Auth::AuthLoginReq msg;
        msg.set_address(address);
        msg.set_captcha(captcha);
        msg.set_sign(sign65Buf1,65);
        msg.set_client_version("1.0.1");
        msg.set_client_type(CLIENT_TYPE_ANDROID);
        ImPdu pdu;
        pdu.SetPBMsg(&msg,CID_AuthLoginReq,0);
        SendPdu(&pdu);
    }
    else{
        if(pPdu->GetCommandId() == CID_HeartBeatNotify){//heart beat
            return;
        }
        else{

        }
    }
}

void AccountManager::onNotify(NativeByteBuffer *buffer,int32_t instanceNum)
{
    if( nullptr != m_delegate){
        buffer->position(0);
        buffer->reuse();
        m_delegate->onNotify(buffer,instanceNum);
    }
}

int AccountManager::SendPduBuf(uint8_t* pduBytes,uint32_t size){
    CImPdu *pPdu = CImPdu::ReadPdu(pduBytes, size);
    DEBUG_D("pdu send n state:%d ",getConnectionState());
    return SendPdu(pPdu);
}

int AccountManager::SendBuf(uint8_t* buf,uint32_t size){
    ssize_t sentLength = size;
    if(m_connection != nullptr){
        NativeByteBuffer *buffer = BuffersStorage::getInstance().getFreeBuffer(size);

    }
    delete buf;
    return sentLength;
}

int AccountManager::_SendPdu(CImPdu* pPdu){
    auto pMsgConn = get_conn(GetAccountId());
    if(pMsgConn != NULL){
        return pMsgConn->SendPdu(pPdu);
    }else{
        return -1;
    }
}

int AccountManager::SendPdu(CImPdu* pPdu){
    DEBUG_D("pdu send n cid=%s len=%d", getActionCommandsName((ActionCommands)pPdu->GetCommandId()).c_str(),pPdu->GetLength());

    if(pPdu->GetReversed() == 1 && pPdu->GetBodyLength() > 0){
        unsigned char shared_secret[32];
        unsigned char iv[16];
        unsigned char aad[16];
        memcpy(shared_secret,hex_to_string(m_sharedKeyHex).data(),32);
        memcpy(iv,hex_to_string(m_ivHex).data(),16);
        memcpy(aad,hex_to_string(m_aadHex).data(),16);

        unsigned char cipherData[1024*1024];
        int cipherDataLen = aes_gcm_encrypt(
                pPdu->GetBodyData(),
                (int)pPdu->GetBodyLength(),
                shared_secret,
                iv,aad,
                cipherData);

        ImPdu pdu1;
        pdu1.SetPBMsg(cipherData,cipherDataLen);
        pdu1.SetServiceId(pPdu->GetServiceId());
        pdu1.SetCommandId(pPdu->GetCommandId());
        pdu1.SetSeqNum(pPdu->GetSeqNum());
        pdu1.SetReversed(pPdu->GetReversed());
        return _SendPdu(&pdu1);
    }else{
        return _SendPdu(pPdu);
    }
}
void AccountManager::setConfigPath(string& configPath){
    g_configPath = configPath;
}

string AccountManager::getEntropyStorePathName() {
    uint32_t accountId = GetAccountId();
    string pathName = "account_" + to_string(accountId);
    string pathName_hash;
    CMd5::MD5_Calculate(pathName.data(),pathName.size(),pathName_hash);
    string path = "account_"+pathName_hash + ".dat";
    return path;
}

string AccountManager::getAccountAddress(){
    if(!m_address.empty()){
        return m_address;
    }else{
        string entropy = GetEntropy();
        if(entropy.empty()){
            upsertEntropy();
        }
        if(!m_address.empty()){
            return m_address;
        }
        PTPWallet::MnemonicHelper::MnemonicResult mnemonicResult = PTPWallet::MnemonicHelper::entropyHexToMnemonic(entropy, "en");
        PTPWallet::HDKey hdKey = PTPWallet::HDKeyEncoder::makeEthRootKey(mnemonicResult.raw.data());
        PTPWallet::HDKeyEncoder::makeEthExtendedKey(hdKey, PTP_HD_PATH);
        DEBUG_D("client pubkey:%s",hdKey.publicKey.to_hex().c_str());
        string address = PTPWallet::HDKeyEncoder::getEthAddress(hdKey);
        hdKey.clear();
        m_address = address;
        return address;
    }
}

string AccountManager::getAddressByEntropy(const string& entropy){
    PTPWallet::MnemonicHelper::MnemonicResult mnemonicResult = PTPWallet::MnemonicHelper::entropyHexToMnemonic(entropy, "en");
    PTPWallet::HDKey hdKey = PTPWallet::HDKeyEncoder::makeEthRootKey(mnemonicResult.raw.data());
    PTPWallet::HDKeyEncoder::makeEthExtendedKey(hdKey, PTP_HD_PATH);
    string address = PTPWallet::HDKeyEncoder::getEthAddress(hdKey);
    hdKey.clear();
    return address;
}

string AccountManager::getWords(){
    upsertEntropy();
    string entropy = GetEntropy();
    PTPWallet::MnemonicHelper::MnemonicResult mnemonicResult = PTPWallet::MnemonicHelper::entropyHexToMnemonic(entropy, "en");
    return mnemonicResult.raw;
}

string AccountManager::genWords(){
    PTPWallet::MnemonicHelper::MnemonicResult mnemonicResult = PTPWallet::MnemonicHelper::generate();
    return mnemonicResult.raw;
}

bool AccountManager::validateWords(const std::string& words) {
    return PTPWallet::MnemonicHelper::validateWords("en", words.data());
}

string AccountManager::getAddressByWords(const std::string& words) {
    uint8_t entropyBytes[BIP39_ENTROPY_LEN_128];
    PTPWallet::MnemonicHelper::mnemonicWordsToEntropy(words.data(),"en",entropyBytes,BIP39_ENTROPY_LEN_128);
    string entropy = ptp_toolbox::data::bytes_to_hex(entropyBytes,16);
    PTPWallet::MnemonicHelper::MnemonicResult mnemonicResult = PTPWallet::MnemonicHelper::entropyHexToMnemonic(entropy, "en");
    PTPWallet::HDKey hdKey = PTPWallet::HDKeyEncoder::makeEthRootKey(mnemonicResult.raw.data());
    PTPWallet::HDKeyEncoder::makeEthExtendedKey(hdKey, PTP_HD_PATH);
    string address = PTPWallet::HDKeyEncoder::getEthAddress(hdKey);
    hdKey.clear();
    return address;
}

bool AccountManager::isAddressExists(const std::string& address) {
    string filePath = getAccountAddressFile(address);
    FILE *accountAddressFile = fopen(filePath.c_str(), "r");
    if (accountAddressFile != nullptr) {
        fclose(accountAddressFile);
        return true;
    }
    return false;
}

bool AccountManager::setWords(const std::string& words){
    uint32_t accountId = GetAccountId();
    string currentConfigPath = getConfigPath();

#ifdef ANDROID
    string path = getEntropyStorePathName();
#else
    string path = currentConfigPath + getEntropyStorePathName();
#endif
    auto config = new Config(0,path);

    uint8_t entropyBytes[BIP39_ENTROPY_LEN_128];
    PTPWallet::MnemonicHelper::mnemonicWordsToEntropy(words.data(),"en",entropyBytes,BIP39_ENTROPY_LEN_128);
    string entropy = ptp_toolbox::data::bytes_to_hex(entropyBytes,16);

    string key = getEncryptEntropyKey(accountId, to_string(accountId));
    char* cipherData;
    uint cipherDataLen = encryptDataUseAecSimple(entropy, key,&cipherData);
    auto *sizeCalculator = new NativeByteBuffer(true);
    sizeCalculator->clearCapacity();
    sizeCalculator->writeString(string(cipherData,cipherDataLen));
    NativeByteBuffer *buffer1 = BuffersStorage::getInstance().getFreeBuffer(sizeCalculator->capacity());
    buffer1->writeString(cipherData);
    config->writeConfig(buffer1);
    buffer1->reuse();
    //DEBUG_D("getEntropy, save to db");
    m_entropy = entropy;
    string address = getAccountAddress();
    FILE *accountAddressFile = fopen(getAccountAddressFile(address).c_str(), "w");
    if (accountAddressFile != nullptr) {
        uint32_t size = 1;
        fwrite(&size, sizeof(uint32_t), 1, accountAddressFile);
        fclose(accountAddressFile);
    }
    return true;
}

void AccountManager::SetInstanceNum(int32_t instanceNum){
    m_instanceNum = instanceNum;
}

void AccountManager::upsertEntropy() {
    uint32_t accountId = GetAccountId();
    string entropy = GetEntropy();
    //DEBUG_D("getEntropy, accountId: %d",accountId);
    if(entropy.empty()){
        string currentConfigPath = getConfigPath();
#ifdef ANDROID
        string path = getEntropyStorePathName();
#else
        string path = currentConfigPath + getEntropyStorePathName();
#endif
        auto config = new Config(0,path);
        NativeByteBuffer *buffer = config->readConfig();
        if (buffer != nullptr) {
            string cipherData = buffer->readString(nullptr);
            string key = getEncryptEntropyKey(accountId, to_string(accountId));
            char* plainData;
            uint32_t plainDataLen = decryptDataUseAecSimple(cipherData, key,&plainData);
            entropy = string(plainData,plainDataLen);
            //DEBUG_D("getEntropy, decrypt from db");
            m_entropy = entropy;
            string address = getAccountAddress();
            m_address = address;
        }else{
            PTPWallet::MnemonicHelper::MnemonicResult mnemonic = PTPWallet::MnemonicHelper::generate();
            uint8_t entropyBytes[BIP39_ENTROPY_LEN_128];
            PTPWallet::MnemonicHelper::mnemonicWordsToEntropy(mnemonic.raw.data(),"en",entropyBytes,BIP39_ENTROPY_LEN_128);
            entropy = ptp_toolbox::data::bytes_to_hex(entropyBytes,16);
            string key = getEncryptEntropyKey(accountId, to_string(accountId));
            char* cipherData;
            uint cipherDataLen = encryptDataUseAecSimple(entropy, key,&cipherData);
            auto *sizeCalculator = new NativeByteBuffer(true);
            sizeCalculator->clearCapacity();
            sizeCalculator->writeString(string(cipherData,cipherDataLen));
            NativeByteBuffer *buffer1 = BuffersStorage::getInstance().getFreeBuffer(sizeCalculator->capacity());
            buffer1->writeString(cipherData);
            config->writeConfig(buffer1);
            buffer1->reuse();
            //DEBUG_D("getEntropy, save to db");
            m_entropy = entropy;
            string address = getAccountAddress();
            m_address = address;
            FILE *accountAddressFile = fopen(getAccountAddressFile(address).c_str(), "w");
            if (accountAddressFile != nullptr) {
                uint32_t size = 1;
                fwrite(&size, sizeof(uint32_t), 1, accountAddressFile);
                fclose(accountAddressFile);
            }
        }
        //DEBUG_D("m_entropy:%s",m_entropy.c_str());
        //DEBUG_D("address:%s",m_address.c_str());
    }
}

string AccountManager::verifyMessageRecoverAddress(const unsigned char *sig_65, const string &message) {
    string msg_data = AccountManager::format_sign_msg_data(message);
    unsigned char sig_64[64];
    memcpy(sig_64,sig_65,64);
    string pub_key_rec = recover_pub_key_from_sig_64(sig_64,sig_65[64], msg_data);
    string address1 = pub_key_to_address(pub_key_rec);
    string address_hex1 = address_to_hex(address1);
    //DEBUG_D("rec address:%s", address_hex1.c_str());
    return address_hex1;
}

string AccountManager::signMessage(const string &message) {
    unsigned char sig_65[65];
    signMessage(message,sig_65);
    return {reinterpret_cast<const char *>(sig_65), 65};
}

void AccountManager::signMessage(const string &message,unsigned char *signOut65) {
    string entropy = GetEntropy();
    if(entropy.empty()){
        upsertEntropy();
        entropy = GetEntropy();
    }
    auto mnemonicRes = PTPWallet::MnemonicHelper::entropyHexToMnemonic(entropy,"en");
    PTPWallet::HDKey hdKey = PTPWallet::HDKeyEncoder::makeBip32RootKey(mnemonicRes.raw.data());
    PTPWallet::HDKeyEncoder::makeExtendedKey(hdKey, PTP_HD_PATH);
    string msg_data = AccountManager::format_sign_msg_data(message);
    DEBUG_D("sign msg_data:%s", msg_data.c_str());
    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
    string address = PTPWallet::HDKeyEncoder::getEthAddress(hdKey);
    DEBUG_D("sign pub key:%s", hdKey.publicKey.to_hex().c_str());
    ecdsa_sign_recoverable(ctx,msg_data,hdKey.privateKey.data(),signOut65);
    hdKey.clear();
}

string AccountManager::signGroupMessage(const string &message,int32_t groupIdx) {
    unsigned char sig_65[65];
    signGroupMessage(message,groupIdx,sig_65);
    return {reinterpret_cast<const char *>(sig_65), 65};
}

void AccountManager::signGroupMessage(const string &message,int32_t groupIdx,unsigned char *signOut65) {
    string entropy = GetEntropy();
    auto mnemonicRes = PTPWallet::MnemonicHelper::entropyHexToMnemonic(entropy,"en");
    PTPWallet::HDKey hdKey = PTPWallet::HDKeyEncoder::makeBip32RootKey(mnemonicRes.raw.data());
    string groupIdxStr = int2string(groupIdx);
    PTPWallet::HDKeyEncoder::makeExtendedKey(hdKey, PTP_GROUP_HD_PATH+groupIdxStr);
    string msg_data = AccountManager::format_sign_msg_data(groupIdxStr+message,SignMsgType_ptp_group);
    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
    string address = PTPWallet::HDKeyEncoder::getEthAddress(hdKey);
    DEBUG_D("signGroupMessage group address:%s", address.c_str());
    ecdsa_sign_recoverable(ctx,msg_data,hdKey.privateKey.data(),signOut65);
    hdKey.clear();
}

bool AccountManager::createShareKey(secp256k1_pubkey *pub_key,unsigned char *shared_key) {
    string entropy = GetEntropy();
    auto mnemonicRes = PTPWallet::MnemonicHelper::entropyHexToMnemonic(entropy,"en");
    PTPWallet::HDKey hdKey = PTPWallet::HDKeyEncoder::makeBip32RootKey(mnemonicRes.raw.data());
    PTPWallet::HDKeyEncoder::makeExtendedKey(hdKey, PTP_HD_PATH);
    string address = PTPWallet::HDKeyEncoder::getEthAddress(hdKey);
    auto ret = ecdh_create_share_key(pub_key, hdKey.privateKey.data(), shared_key);
    DEBUG_D("client accountId: %d", GetAccountId());
    DEBUG_D("client entropy: %s", entropy.c_str());
    DEBUG_D("client address: %s", address.c_str());
    DEBUG_D("client pubKey: %s", bytes_to_hex_string(hdKey.publicKey.data(),33).c_str());
    DEBUG_D("client prvKey: %s", bytes_to_hex_string(hdKey.privateKey.data(),32).c_str());
    hdKey.clear();
    return ret;
}

void AccountManager::setConnectionState(ConnectionState state){
    m_connectionState = state;
}

void AccountManager::onHeartBeat(){
    if(isConnected() || isConnecting()){
        ImPdu pdu;
        pdu.SetPBMsg(nullptr);
        pdu.SetServiceId(7);
        pdu.SetCommandId(1793);
        pdu.SetReversed(0);
        //DEBUG_D("heart beat...");
        CImPdu *pPdu = CImPdu::ReadPdu(pdu.GetBuffer(), pdu.GetLength());
        _SendPdu(pPdu);
    }
}

void AccountManager::closeSocket(){
    if(isConnected()){
        ImPdu pdu;
        pdu.SetPBMsg(nullptr);
        pdu.SetServiceId(1);
        pdu.SetCommandId(259);//CID_AuthLogoutReq
        pdu.SetReversed(0);
        DEBUG_D("onLogout: %d",GetAccountId());
        SendPduBuf(pdu.GetBuffer(),pdu.GetLength());
    }
}

void AccountManager::onConnectionStateChanged(ConnectionState state, uint32_t accountId)
{
    AccountManager::getInstance(accountId).setConnectionState(state);
    int32_t notifySize;
    if(state == ConnectionStateConnecting){
        string address = AccountManager::getInstance(accountId).getAccountAddress();
        notifySize = NativeInvokeHeaderSize + getStringSize(address);
        NativeByteBuffer *buffer = BuffersStorage::getInstance().getFreeBuffer(notifySize);
        buffer->writeInt32((int32_t) notifySize);
        buffer->writeInt32(NativeInvokeType_SWITCH_ACCOUNT);
        buffer->writeInt32(NativeInvokeDefaultSepNum);
        buffer->writeInt32((int32_t)accountId);
        buffer->writeString(address);
        AccountManager::onNotify(buffer,0);
    }
    notifySize = NativeInvokeHeaderSize + NativeInvokeIntSize;
    NativeByteBuffer *buffer1 = BuffersStorage::getInstance().getFreeBuffer(notifySize);
    buffer1->writeInt32((int32_t) notifySize);
    buffer1->writeInt32(NativeInvokeType_STATE_CHANGE);
    buffer1->writeInt32(NativeInvokeDefaultSepNum);
    buffer1->writeInt32((int32_t)accountId);
    buffer1->writeInt32(state);
    AccountManager::onNotify(buffer1,0);
}

ConnectionState AccountManager::getConnectionState() {
    return m_connectionState;
}

void AccountManager::handleInvoke(NativeByteBuffer *task,uint32_t instanceNum){
    int32_t len = task->readInt32(nullptr);
    int32_t type = task->readInt32(nullptr);
    int32_t seqNum = task->readInt32(nullptr);
    int32_t accountId = task->readInt32(nullptr);
    DEBUG_D("handleInvoke type=%d",type);
    uint32_t notifySize = 0;
    NativeByteBuffer *buffer;

    switch (type) {
        case NativeInvokeType_CLOSE_SOCKET:{
            AccountManager::getInstance(accountId).closeSocket();
            break;
        }
        case NativeInvokeType_SWITCH_ACCOUNT:
        {
            string address = AccountManager::getInstance(accountId).getAccountAddress();
            notifySize = NativeInvokeHeaderSize + getStringSize(address);
            buffer = BuffersStorage::getInstance().getFreeBuffer(notifySize);
            buffer->writeInt32((int32_t) notifySize);
            buffer->writeInt32(NativeInvokeType_SWITCH_ACCOUNT);
            buffer->writeInt32(seqNum);
            buffer->writeInt32((int32_t)accountId);
            buffer->writeString(address);
            AccountManager::setCurrentAccountId(accountId);
            break;
        }
        case NativeInvokeType_HEART_BEAT:
        {
            buffer = BuffersStorage::getInstance().getFreeBuffer(NativeInvokeHeaderSize);
            buffer->writeInt32((int32_t) notifySize);
            buffer->writeInt32(type);
            buffer->writeInt32(seqNum);
            buffer->writeInt32(accountId);
            break;
        }
        case NativeInvokeType_SIGN_GROUP_MSG:
        {
//            len = len - NativeInvokeHeaderSize;
//            int32_t groupIdx = task->readInt32(nullptr);
//            ByteArray* captchaByteArray = task->readBytes(len - 4,nullptr);
//            uint8_t *captchaBytes = captchaByteArray->bytes;
//            string captcha = string((char *)captchaBytes, len - 4);
//            DEBUG_D("NativeInvokeType_SIGN_GROUP_MSG len=%d,captcha=%s,groupIdx=%d",len,captcha.c_str(),groupIdx);
//            string sign = AccountManager::getInstance(accountId).signGroupMessage(captcha,groupIdx);
//            auto* signBytes = (uchar_t *) sign.data();
//            notifySize = NativeInvokeHeaderSize + sign.size();
//
//            buffer = BuffersStorage::getInstance().getFreeBuffer(notifySize);
//            buffer->writeInt32((int32_t) notifySize);
//            buffer->writeInt32(type);
//            buffer->writeInt32(seqNum);
//            buffer->writeInt32(accountId);
//            buffer->writeBytes(signBytes,sign.size());
            break;
        }
        case NativeInvokeType_PDU_DOWNLOAD:
        case NativeInvokeType_PDU_UPLOAD:
        {
            len = len - NativeInvokeHeaderSize;
            ByteArray* pduByteArray = task->readBytes(len,nullptr);
//            uint8_t *pduBytes = pduByteArray->bytes;
//            while (true){
//                DEBUG_D("socketSend send:%d",len);
//                string ip = MSFS_SERVER_IP_1;
//                uint16_t port = MSFS_SERVER_PORT_1;
//                CSimpleBuffer request = *new CSimpleBuffer();
//                CSimpleBuffer response = *new CSimpleBuffer();
//                request.Write(pduBytes,len);
//                int ret = socketSend(ip.c_str(),port,&request,&response);
//                if(ret > 0){
//                    break;
//                }
//                notifySize = NativeInvokeHeaderSize + response.GetLength();
//                buffer = BuffersStorage::getInstance().getFreeBuffer(notifySize);
//                buffer->writeInt32(notifySize);
//                buffer->writeInt32(NativeInvokeType_PDU_RECV);
//                buffer->writeInt32(seqNum);
//                buffer->writeInt32(accountId);
//                buffer->writeBytes(response.GetBuffer(),response.GetLength());
//                break;
//            }
            break;
        }
        case NativeInvokeType_PDU_SEND:
        {
//            len = len - NativeInvokeHeaderSize;
//            ByteArray* pduByteArray = task->readBytes(len,nullptr);
//            AccountManager::getInstance(accountId).SendPduBuf(pduByteArray->bytes,len);
            break;
        }
        case NativeInvokeType_SET_WORDS:
        {
//            int32_t accountId1 = task->readInt32(nullptr);
//
//            len = len - NativeInvokeHeaderSize - NativeInvokeIntSize;
//            ByteArray* wordsByteArray = task->readBytes(len,nullptr);
//            uint8_t *wordsBytes = wordsByteArray->bytes;
//            string words1 = string((char *)wordsBytes, len);
//            int res_code = 1;
//            bool res_valid = AccountManager::validateWords(words1);
//            if(res_valid){
//                string address = AccountManager::getAddressByWords(words1);
//                bool res = AccountManager::isAddressExists(address);
//                if(!res){
//                    bool res_setWords = AccountManager::getInstance(accountId1).setWords(words1);
//                    if(res_setWords){
//                        res_code = 0;
//                    }else{
//                        res_code = 3;
//                    }
//                }else{
//                    res_code = 2;
//                }
//            }
//            notifySize = NativeInvokeHeaderSize + NativeInvokeIntSize + NativeInvokeIntSize;
//            buffer = BuffersStorage::getInstance().getFreeBuffer(notifySize);
//            buffer->writeInt32((int32_t) notifySize);
//            buffer->writeInt32(type);
//            buffer->writeInt32(seqNum);
//            buffer->writeInt32(accountId);
//            buffer->writeInt32(accountId1);
//            buffer->writeInt32(res_code);
            break;
        }
        case NativeInvokeType_GET_WORDS:
        {
            int32_t accountId1 = task->readInt32(nullptr);
            string words = AccountManager::getInstance(accountId1).getWords();
            notifySize = NativeInvokeHeaderSize + getStringSize(words);
            buffer = BuffersStorage::getInstance().getFreeBuffer(notifySize);
            buffer->writeInt32((int32_t) notifySize);
            buffer->writeInt32(type);
            buffer->writeInt32(seqNum);
            buffer->writeInt32(accountId);
            buffer->writeString(words);
            break;
        }
        case NativeInvokeType_GEN_WORDS:
        {
            string words = AccountManager::genWords();
            notifySize = NativeInvokeHeaderSize + getStringSize(words);
            buffer = BuffersStorage::getInstance().getFreeBuffer(notifySize);
            buffer->writeInt32((int32_t) notifySize);
            buffer->writeInt32(type);
            buffer->writeInt32(seqNum);
            buffer->writeInt32(accountId);
            buffer->writeString(words);
            break;
        }
        default:
            break;
    }

    if(notifySize > 0){
        AccountManager::onNotify(buffer,instanceNum);
    }
}

string AccountManager::format_sign_msg_data(const string &message,SignMsgType signMsgType) {
    if(signMsgType == SignMsgType_ptp){
        return string("\x19") + PTP_Signed_Message + to_string(message.size()) + message;
    }else if(signMsgType == SignMsgType_ptp_group){
        return string("\x19") + PTP_GROUP_Signed_Message + to_string(message.size()) + message;
    }else{
        return format_eth_msg_data(message);
    }
}

bool AccountManager::isConnected() {
    return m_connectionState == ConnectionLogged || m_connectionState == ConnectionStateConnected;
}

bool AccountManager::isClosed(){
    return m_connectionState == ConnectionStateClosed;

}

bool AccountManager::isConnecting(){
    return m_connectionState == ConnectionStateConnecting;
}


