#include "ptp_net/AccountManager.h"

#include <sys/socket.h>
#include "ptp_wallet/MnemonicHelper.h"
#include "ptp_wallet/HDKey.h"
#include "ptp_crypto/keccak.h"
#include "ptp_crypto/secp256k1_helpers.h"
#include "ptp_crypto/ptp_helpers.h"
#include "ptp_crypto/aes_encryption.h"
#include "Config.h"
#include "BuffersStorage.h"
#include "ptp_common/AesSimple.h"

static uint32_t		m_current_account_id = 0;
static AccountsMap_t g_accounts_map;
static SocketAccountMap_t g_socket_account_map;
static string g_configPath;
static ConnectiosManagerDelegate* m_delegate;

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
        }
    }
}

AccountManager AccountManager::getCurrentAccount(){
    return AccountManager::getInstance(AccountManager::getCurrentAccountId());
}

void AccountManager::onConnecting(int32_t instanceNum){
    DEBUG_D("onConnecting instanceNum=%d",instanceNum);
    if(instanceNum != 0){
        return;
    }
    AccountManager::getCurrentAccount().upsertEntropy();
    AccountManager::onConnectionStateChanged(ConnectionStateConnecting,AccountManager::getCurrentAccountId());
}

void AccountManager::onConnect(int32_t instanceNum,int socketFd){
    DEBUG_D("onConnect instanceNum=%d,socketFd=%d",instanceNum,socketFd);
    if(instanceNum != 0 || socketFd == -1){
        return;
    }
    for (auto it = g_socket_account_map.begin(); it != g_socket_account_map.end(); ) {
        if(it->second.GetAccountId() == AccountManager::getCurrentAccountId()){
            g_socket_account_map.erase(it->first);
        }
    }
    g_socket_account_map.insert(make_pair(socketFd,AccountManager::getCurrentAccount()));
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
                it->second.handlePdu(buff,size);
                break;
            }
        }
    }
}

void AccountManager::handlePdu(NativeByteBuffer * buff,uint32_t bufSize){

    buff->position(0);
    uint8_t pduBuf[bufSize];
    buff->copyBuffer(0,bufSize,pduBuf);
    uint32_t pduLen = CByteStream::ReadUint32(pduBuf);
    auto pPdu = CImPdu::ReadPdu(pduBuf, pduLen);

    if(bufSize >= pduLen){
        if(pPdu->GetCommandId() == 1793){//heart beat
            return;
        }
        DEBUG_D("onRead data: %s, len: %d", ptp_toolbox::data::bytes_to_hex(pduBuf,pduLen).c_str(),pduLen);
        DEBUG_D("onRead GetCommandId: %d,pduLen:%d",pPdu->GetCommandId(),pduLen);

        NativeByteBuffer *buffer;
        if(pPdu->GetReversed() == 1 ){
            if(m_connectionState != ConnectionLogged){
                m_connectionState = ConnectionLogged;
                AccountManager::onConnectionStateChanged(m_connectionState,GetAccountId());
            }

            unsigned char shared_secret[32];
            unsigned char iv[16];
            unsigned char aad[16];
            memcpy(shared_secret,hex_to_string(m_sharedKeyHex).data(),32);
            memcpy(iv,hex_to_string(m_ivHex).data(),16);
            memcpy(aad,hex_to_string(m_aadHex).data(),16);

            auto cipherData = pPdu->GetBodyData();
            auto cipherDataLen = pPdu->GetBodyLength();

            unsigned char decData[1024];
            int decLen = aes_gcm_decrypt(
                    cipherData, (int)cipherDataLen,
                    shared_secret, iv,
                    aad,
                    decData);
            DEBUG_D("decLen: %d",decLen);
            DEBUG_D("decData: %s",ptp_toolbox::data::bytes_to_hex(decData,decLen).c_str());

            if (decLen > 0) {
                CImPdu pdu;
                pdu.SetPBMsg(decData,decLen);
                pdu.SetServiceId(pPdu->GetServiceId());
                pdu.SetCommandId(pPdu->GetCommandId());
                pdu.SetSeqNum(pPdu->GetSeqNum());
                pdu.SetReversed(0);
                pPdu = CImPdu::ReadPdu(pdu.GetBuffer(), pdu.GetLength());
                memset(decData, 0, sizeof(decData));
                pduLen = pPdu->GetLength();
                DEBUG_D("pduLen decrypt: %d",pduLen);
                DEBUG_D("GetBodyLength: %d",pPdu->GetBodyLength());
                DEBUG_D("GetBodyData: %s", bytes_to_hex_string(pPdu->GetBodyData(),pPdu->GetBodyLength()).c_str());
            }else{
                DEBUG_E("error decrypt body,cid:%d",pPdu->GetCommandId() );
                return;
            }
        }

        if(pPdu->GetCommandId() == 262){//auth captcha
            auto buf = pPdu->GetBodyData();
            unsigned char captchaBuff[6] = { 0 };
            memcpy(captchaBuff,buf + 2,6);
            unsigned char addressBuff[20] = { 0 };
            memcpy(addressBuff,buf + 2 + 6 + 2,20);
            DEBUG_D("server address: %s,captcha:%s", ptp_toolbox::data::bytes_to_hex(addressBuff,20).c_str(),captchaBuff);

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
            DEBUG_D("signOut65: %s", ptp_toolbox::data::bytes_to_hex(sign65Buf,65).c_str());

            std::string captcha((char*) captchaBuff,6);

            string srv_address_hex;
            unsigned char pub_key_33[33] = {0};
            string msg_data = format_eth_msg_data(captcha);
            auto ret = recover_pub_key_from_sig_65(sign65Buf, msg_data, pub_key_33,srv_address_hex);
            if(!ret){
                DEBUG_E("recover_pub_key_from_sig_65 error!");
                return;
            }
            if(srv_address_hex.substr(2) != ptp_toolbox::data::bytes_to_hex(addressBuff,20)){
                DEBUG_E("srv_address_hex error!");
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
            uint32_t accountId = GetAccountId();
            createShareKey(&pub_key_raw,shared_secret);
            DEBUG_D("shared_secret: %s", ptp_toolbox::data::bytes_to_hex(shared_secret,32).c_str());
            m_sharedKeyHex = ptp_toolbox::data::bytes_to_hex(shared_secret,32);
            string address = getAccountAddress();
            string sign = signMessage(captcha);
            auto *sizeCalculator = new NativeByteBuffer(true);
            sizeCalculator->clearCapacity();
            sizeCalculator->writeInt32(NativeInvokeHeaderSize);
            sizeCalculator->writeInt32(NativeInvokeType_SIGN_AUTH_CAPTCHA);
            sizeCalculator->writeInt32(0);
            sizeCalculator->writeInt32((int32_t)GetAccountId());
            sizeCalculator->writeString(captcha);
            sizeCalculator->writeString(address);
            auto* signBytes = (uchar_t *) sign.data();
            sizeCalculator->writeBytes(signBytes,sign.size());
            uint32_t size = sizeCalculator->capacity();
            buffer = BuffersStorage::getInstance().getFreeBuffer(size);
            buffer->writeInt32((int32_t)size);
            buffer->writeInt32(NativeInvokeType_SIGN_AUTH_CAPTCHA);
            buffer->writeInt32(0);
            buffer->writeInt32((int32_t)GetAccountId());
            buffer->writeString(captcha);
            buffer->writeString(address);
            buffer->writeBytes(signBytes,sign.size());
        }else{
            buffer = BuffersStorage::getInstance().getFreeBuffer(NativeInvokeHeaderSize + pduLen );
            buffer->writeInt32(NativeInvokeHeaderSize + pduLen);
            buffer->writeInt32(NativeInvokeType_PDU_RECV);
            buffer->writeInt32(0);
            buffer->writeInt32((int32_t)GetAccountId());
            buffer->writeBytes(pPdu->GetBuffer(),pduLen);
        }

        if(m_delegate != nullptr){
            AccountManager::onNotify(buffer);
        }
    }
}

void AccountManager::onNotify(NativeByteBuffer *buffer)
{
    if( nullptr != m_delegate){
        buffer->position(0);
        buffer->reuse();
        m_delegate->onNotify(buffer);
    }
}

int AccountManager::SendPduBuf(uint8_t* pduBytes,uint32_t size){
    auto pPdu = CImPdu::ReadPdu(pduBytes, size);
    return SendPdu(pPdu);
}

int AccountManager::_SendPdu(CImPdu* pPdu){
    ssize_t sentLength = -1;

    for (auto it = g_socket_account_map.begin(); it != g_socket_account_map.end(); ) {
        if(it->second.GetAccountId() == GetAccountId()){
            if ((sentLength = send(it->first, pPdu->GetBuffer(), pPdu->GetLength(), 0)) < 0) {
                if (LOGS_ENABLED) DEBUG_D("connection(%p) send failed", this);
            }
            break;
        }
    }
    delete pPdu;
    pPdu = NULL;
    return sentLength;
}

int AccountManager::SendPdu(CImPdu* pPdu){
    DEBUG_D("pdu send n cid=%d len=%d ",pPdu->GetCommandId(),pPdu->GetLength());
    if(pPdu->GetCommandId() == 1281){
        DEBUG_D("CID_FileImgUploadReq_VALUE");
    }
    int res;
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

        CImPdu pdu1;
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
        PTPWallet::HDKeyEncoder::makeEthExtendedKey(hdKey, "m/44'/60'/0'/0/0");
        string address = PTPWallet::HDKeyEncoder::getEthAddress(hdKey);
        hdKey.clear();
        m_address = address;
        return address;
    }
}

string AccountManager::getAddressByEntropy(const string& entropy){
    PTPWallet::MnemonicHelper::MnemonicResult mnemonicResult = PTPWallet::MnemonicHelper::entropyHexToMnemonic(entropy, "en");
    PTPWallet::HDKey hdKey = PTPWallet::HDKeyEncoder::makeEthRootKey(mnemonicResult.raw.data());
    PTPWallet::HDKeyEncoder::makeEthExtendedKey(hdKey, "m/44'/60'/0'/0/0");
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
    PTPWallet::HDKeyEncoder::makeEthExtendedKey(hdKey, "m/44'/60'/0'/0/0");
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
    DEBUG_D("getEntropy, save to db");
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

int32_t AccountManager::GetInstanceNum(int32_t instanceNum){
    return m_instanceNum;
}

void AccountManager::upsertEntropy() {
    uint32_t accountId = GetAccountId();
    string entropy = GetEntropy();
    DEBUG_D("getEntropy, accountId: %d",accountId);
    if(!entropy.empty()){
        DEBUG_D("getEntropy, from hash map");
    }else{
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
            DEBUG_D("getEntropy, decrypt from db");
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
            DEBUG_D("getEntropy, save to db");
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
        DEBUG_D("address:%s",m_address.c_str());
    }
}

string AccountManager::verifyMessageRecoverAddress(const unsigned char *sig_65, const string &message) {
    string msg_data = format_eth_msg_data(message);
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
    auto mnemonicRes = PTPWallet::MnemonicHelper::entropyHexToMnemonic(entropy,"en");
    PTPWallet::HDKey hdKey = PTPWallet::HDKeyEncoder::makeBip32RootKey(mnemonicRes.raw.data());
    PTPWallet::HDKeyEncoder::makeExtendedKey(hdKey, "m/44'/60'/0'/0/0");
    string msg_data = format_eth_msg_data(message);
    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
    string address = PTPWallet::HDKeyEncoder::getEthAddress(hdKey);
    //DEBUG_D("org address:%s", address.c_str());
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
    PTPWallet::HDKeyEncoder::makeExtendedKey(hdKey, "m/44'/60'/0'/0/"+groupIdxStr);
    string msg_data = format_eth_msg_data(groupIdxStr+message);
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
    PTPWallet::HDKeyEncoder::makeExtendedKey(hdKey, "m/44'/60'/0'/0/0");
    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
    string address = PTPWallet::HDKeyEncoder::getEthAddress(hdKey);
    //DEBUG_D("org address:%s", address.c_str());
    auto ret = ecdh_create_share_key(pub_key, hdKey.privateKey.data(), shared_key);
    secp256k1_context_destroy(ctx);
    hdKey.clear();
    return ret;
}

void AccountManager::setConnectionState(ConnectionState state){
    m_connectionState = state;
}

void AccountManager::onHeartBeat(){
    if(m_connectionState == ConnectionLogged){
        CImPdu pdu;
        pdu.SetPBMsg(nullptr);
        pdu.SetServiceId(7);
        pdu.SetCommandId(1793);
        DEBUG_D("heart beat...");
        SendPdu(&pdu);
    }
}

void AccountManager::onConnectionStateChanged(ConnectionState state, uint32_t accountId)
{
    AccountManager::getInstance(accountId).setConnectionState(state);
    if( nullptr != m_delegate){
        m_delegate->onConnectionStateChanged(state,0);
        if(state == ConnectionStateConnecting){
            string address = AccountManager::getInstance(accountId).getAccountAddress();
            int32_t notifySize = NativeInvokeHeaderSize + getStringSize(address);
            NativeByteBuffer *buffer = BuffersStorage::getInstance().getFreeBuffer(notifySize);
            buffer->writeInt32((int32_t) notifySize);
            buffer->writeInt32(NativeInvokeType_CHANGE_ACCOUNT);
            buffer->writeInt32(NativeInvokeDefaultSepNum);
            buffer->writeInt32((int32_t)accountId);
            buffer->writeString(address);
            AccountManager::onNotify(buffer);
        }
    }
}

ConnectionState AccountManager::getConnectionState() {
    return m_connectionState;
}



