#include <gtest/gtest.h>
#include "ptp_wallet/MnemonicHelper.h"
#include "ptp_wallet/HDKey.h"
#include "ptp_crypto/keccak.h"
#include "ptp_crypto/secp256k1_helpers.h"
#include "ptp_crypto/ptp_helpers.h"
#include "ptp_common/Logger.h"


TEST(ptp_wallet, bip39_get_languages_size) {
    ASSERT_EQ((size_t) bip39_get_languages_size(),7);
}

TEST(ptp_wallet, GenerateMnemonics) {
    PTPWallet::MnemonicHelper::MnemonicResult m1 = PTPWallet::MnemonicHelper::generate();
    PTPWallet::MnemonicHelper::MnemonicResult m2 = PTPWallet::MnemonicHelper::generate();
    PTPWallet::MnemonicHelper::MnemonicResult m3 = PTPWallet::MnemonicHelper::generate();

    ASSERT_TRUE(PTPWallet::MnemonicHelper::validateWords("en", m1.raw.data()));
    ASSERT_TRUE(PTPWallet::MnemonicHelper::validateWords("en", m2.raw.data()));
    ASSERT_TRUE(PTPWallet::MnemonicHelper::validateWords("en", m3.raw.data()));
    std::cout << m1.raw << std::endl;
    std::cout << m2.raw << std::endl;
    std::cout << m3.raw << std::endl;
    ASSERT_STRNE(m1.raw.c_str(), m2.raw.c_str());
    ASSERT_STRNE(m1.raw.c_str(), m3.raw.c_str());
    ASSERT_STRNE(m2.raw.c_str(), m3.raw.c_str());
}

TEST(ptp_wallet, genWords) {
    const char* words = "opinion rally million case address approve upper scrub exhibit shock ostrich calm";
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
    ASSERT_EQ("0x9b762a3291903615bbc60e4f98ca7390", bytes_to_hex_string(entropyBytes,16));

    PTPWallet::MnemonicHelper::MnemonicResult mnemonicResult1 = PTPWallet::MnemonicHelper::entropyToMnemonic(entropyBytes,"en",BIP39_ENTROPY_LEN_128);
    DEBUG_D("words: %s",mnemonicResult1.raw.c_str());
    ASSERT_EQ(mnemonicResult1.raw,words);

    auto entropy = ptp_toolbox::data::bytes_to_hex(entropyBytes,16);
    ASSERT_EQ("9b762a3291903615bbc60e4f98ca7390", entropy);

    PTPWallet::MnemonicHelper::MnemonicResult mnemonicResult21 = PTPWallet::MnemonicHelper::entropyHexToMnemonic(entropy, "en");
    ASSERT_EQ(words,mnemonicResult21.raw);
    PTPWallet::HDKey hdKey1 = PTPWallet::HDKeyEncoder::makeEthRootKey(mnemonicResult21.raw.data());
    PTPWallet::HDKeyEncoder::makeEthExtendedKey(hdKey1, "m/44'/60'/0'/0/0");
    DEBUG_D("rootHdKey1: %s",hdKey1.privateKey.to_hex().c_str());
    string address11 = PTPWallet::HDKeyEncoder::getEthAddress(hdKey1);
    ASSERT_EQ(address11,"0xf2472d9e07c721da4bf74ddd5c587ca7f5b3ee60");
    DEBUG_D("address: %s",address11.c_str());
    DEBUG_D("entropy: %s",entropy.c_str());
    PTPWallet::HDKey rootHdKey = PTPWallet::HDKeyEncoder::makeEthRootKey(words);
    DEBUG_D("rootHdKey: %s",rootHdKey.privateKey.to_hex().c_str());

    PTPWallet::HDKeyEncoder::makeEthExtendedKey(rootHdKey, "m/44'/60'/0'/0/0");
    DEBUG_D("ext prvKey: %s",rootHdKey.privateKey.to_hex().c_str());
    DEBUG_D("ext pubKey: %s",rootHdKey.publicKey.to_hex().c_str());
    DEBUG_D("ext pubKey64: %s",rootHdKey.publicKey64.to_hex().c_str());
    string address = PTPWallet::HDKeyEncoder::getEthAddress(rootHdKey);
    DEBUG_D("org address:%s", address.c_str());

    string message = "message test";
    string msg_data = format_eth_msg_data(message);
    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    unsigned char sig_65[65];
    ecdsa_sign_recoverable(ctx,msg_data,rootHdKey.privateKey.data(),sig_65);
    rootHdKey.clear();
    DEBUG_D("sig_65 %s", ptp_toolbox::data::bytes_to_hex(sig_65,65).c_str());

    string pub_key_rec1 = recover_pub_key_from_sig_65(sig_65,msg_data);
    DEBUG_D("rec pub_key_rec:%s", ptp_toolbox::data::bytes_to_hex(reinterpret_cast<const uint8_t *>(pub_key_rec1.data()), 64).c_str());

    string address12 = pub_key_to_address(pub_key_rec1);
    string address_hex12 = address_to_hex(address12);
    DEBUG_D("rec address:%s", address_hex12.c_str());

    unsigned char sig_64[64];
    memcpy(sig_64,sig_65,64);
    string pub_key_rec = recover_pub_key_from_sig_64(sig_64,sig_65[64], msg_data);
    DEBUG_D("rec pub_key_rec:%s", ptp_toolbox::data::bytes_to_hex(reinterpret_cast<const uint8_t *>(pub_key_rec.data()), 64).c_str());

    string address1 = pub_key_to_address(pub_key_rec);
    string address_hex1 = address_to_hex(address1);
    DEBUG_D("rec address:%s", address_hex1.c_str());

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
