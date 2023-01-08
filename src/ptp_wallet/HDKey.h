#ifndef BIP39_KEY_ENCODER_H
#define BIP39_KEY_ENCODER_H

#include "ptp_wallet/MnemonicHelper.h"
#include "ptp_wallet/utils.h"
#include "ptp_crypto/secp256k1.h"

namespace PTPWallet {

struct BTCNetwork {
    std::string name;
    std::string bech32;
    uint32_t bip32[2];
    uint8_t pubKeyHash;
    uint8_t scriptHash;
    uint8_t wif;
};

static const BTCNetwork MainNet = {
    "bitcoin",
    "bc",
    {0x0488b21e, 0x0488ade4},
    0x00,
    0x05,
    0x80};

static const BTCNetwork TestNet = {
    "testnet",
    "tb",
    {0x043587cf, 0x04358394},
    0x6f,
    0xc4,
    0xef};

static const std::string MASTER_SECRET = "Bitcoin seed";

class Derivation {
public:
    Derivation(const std::string& path);
    Derivation(std::string&& path);
    Derivation(const char* path);

    std::string path;
};

class HDKey {
public:
    HDKey();
    ~HDKey();
    HDKey(const HDKey& other);
    HDKey(HDKey&& other) noexcept;
    HDKey& operator=(PTPWallet::HDKey other);

    void clear();

    bytes_array<33> publicKey;
    bytes_array<65> publicKey65;
    bytes_array<64> publicKey64;
    bytes_array<32> privateKey;
    bytes_array<32> chainCode;
    bip32_key extPrivateKey;
    bip32_key extPublicKey;
    BTCNetwork net;
    uint8_t depth;
    uint32_t index;
    uint32_t fingerprint;
    const curve_info* curve;
};

class HDKeyEncoder {
public:
    static bytes_64 makeBip39Seed(const std::string& mnemonicWords);
    static bytes_64 makeBip39Seed(const std::vector<std::string>& mnemonicWords);
    static HDKey makeBip32RootKey(const char* mnemonic, BTCNetwork net = PTPWallet::MainNet);
    static HDKey makeBip32RootKey(const bytes_64& seed, BTCNetwork net = PTPWallet::MainNet);
    static void makeExtendedKey(HDKey& rootKey, const Derivation& derivation);
    static HDKey makeEthRootKey(const char* mnemonic);
    static HDKey makeEthRootKey(const bytes_64& seed);
    static void makeEthExtendedKey(HDKey& rootKey, const Derivation& derivation);
    static string getEthAddress(HDKey& rootKey);
    static string formatEthMsgData(const string& message);

private:
    static void derive(HDKey& key, uint32_t index);
    static HDKey fromSeed(const bytes_data& seed);
    static HDKey fromSeed(const char* seed);
    static HDKey fromSeed(const uint8_t* seed, size_t seedLength);
    static void derivePath(HDKey& key, const std::string& path, bool priv = true);
    static uint32_t fetchFingerprint(HDKey& key);
    static void fillPublicKey(HDKey& key);
    static void fillPublicKey65(HDKey& key);
    static void fillPublicKey64(HDKey& key);
    static void serialize(HDKey& key, uint32_t fingerprint, uint32_t version, bool publicKey = false);
};
}

#endif