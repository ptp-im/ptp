#include "ptp_net/AccountManager.h"
#include "ptp_net/MsgConn.h"

#include "ptp_wallet/MnemonicHelper.h"
#include "ptp_wallet/HDKey.h"
#include "ptp_crypto/keccak.h"
#include "ptp_crypto/secp256k1_helpers.h"
#include "ptp_crypto/ptp_helpers.h"
#include "Config.h"
#include "BuffersStorage.h"

#include "ptp_common/AesSimple.h"

static AccountsMap_t g_accounts_map;
static string g_configPath = "/tmp";

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

AccountManager::AccountManager(uint32_t accountId) {
    m_accountId = accountId;
}

AccountManager::~AccountManager() {

}

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

uint32_t AccountManager::getCurrentAccountId(){
    return get_current_account_id();
}

void AccountManager::setCurrentAccountId(uint32_t accountId){
    set_current_account_id(accountId);
}

void AccountManager::sendPdu(uint8_t* pduBytes,uint32_t size){
    CMsgConn* pMsgConn = get_msg_conn(GetAccountId());
    if(pMsgConn != nullptr){
        auto pPdu = CImPdu::ReadPdu((uchar_t*)pduBytes, (int)size);
        pMsgConn->SendPdu(pPdu);
        delete pPdu;
        pPdu = NULL;
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


