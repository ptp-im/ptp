#include <gtest/gtest.h>

#include "ptp_crypto/keccak.h"
#include "ptp_crypto/secp256k1_helpers.h"
#include "ptp_crypto/crypto_helpers.h"
#include "ptp_global/Logger.h"

TEST(ptp_crypto, keccak) {
    uint8_t actualHash[32];
    std::string message = "836B35A026743E823A90A0EE3B91BF615C6A757E2B60B9E1DC1826FD0DD16106F7BC1E8179F665015F43C6C81F39062FC2086ED849625C06E04697698B21855E";
    Keccak256::getHash(reinterpret_cast<const uint8_t *>(message.data()), message.size(), actualHash);
    uint8_t actualHash1[32];
    std::string message1 = "836B35A026743E823A90A0EE3B91BF615C6A757E2B60B9E1DC1826FD0DD16106F7BC1E8179F665015F43C6C81F39062FC2086ED849625C06E04697698B21855E";
    Keccak256::getHash(message1, actualHash1);
    DEBUG_D("%s",actualHash1);
    ASSERT_EQ(actualHash[0],actualHash1[0]);
}

TEST(ptp_crypto, signRecover) {
    unsigned char sig_64[64];
    unsigned char sig_65[65];
    unsigned char prv_key[32];
    unsigned char pub_key[65];

    string message = "test_message";
    string msg_data = format_eth_msg_data(message);
    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
    gen_ec_key_pair(ctx,prv_key,pub_key);
    std::cout << "prv_key:" + bytes_to_hex_string(prv_key,32) << std::endl;

    string pub_key_hex = pub_key_to_hex(pub_key);
    std::cout << "org pub_key_hex:" + pub_key_hex << std::endl;
    secp256k1_context *ctx1 = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
    ecdsa_sign_recoverable(ctx1,msg_data,prv_key,sig_65);
    memcpy(sig_64,sig_65,64);
    string pub_key_rec = recover_pub_key_from_sig_64(sig_64,sig_65[64], msg_data);
    string pub_key_rec_hex = bytes_to_hex_string(reinterpret_cast<const uint8_t *>(pub_key_rec.data()), pub_key_rec.size());
    std::cout << "rec pub_key_hex:" + pub_key_rec_hex << std::endl;
    string address_hex = address_to_hex(pub_key_to_address(pub_key_rec));
    std::cout << "address_hex:" + address_hex << std::endl;
    auto pub_key_rec1 = recover_pub_key_from_sig_65(sig_65,msg_data);
    string pub_key_rec_hex1 = bytes_to_hex_string(reinterpret_cast<const uint8_t *>(pub_key_rec1.data()), pub_key_rec1.size());
    std::cout << "rec pub_key_hex:" + pub_key_rec_hex1 << std::endl;
    string address_hex1 = address_to_hex(pub_key_to_address(pub_key_rec1));
    std::cout << "address_hex:" + address_hex1 << std::endl;
    ASSERT_EQ(address_hex1,address_hex);
    ASSERT_EQ(pub_key_hex,pub_key_rec_hex);
    ASSERT_EQ(pub_key_hex,pub_key_rec_hex1);
}

TEST(ptp_crypto, signRecover11) {
    unsigned char sig_64[64];
    unsigned char sig_65[65];
    unsigned char prv_key[32];
    unsigned char pub_key[65];
    unsigned char pub_key_64[64];

    string message = "test_message";
    string msg_data = format_eth_msg_data(message);
    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    gen_ec_key_pair(ctx,prv_key,pub_key);
    std::cout << "prv_key:" + bytes_to_hex_string(prv_key,32) << std::endl;
    string pub_key_hex = pub_key_to_hex(pub_key);
    std::cout << "org pub_key_hex:" + pub_key_hex << std::endl;

    for (int i = 1; i < 65; ++i) {
        pub_key_64[i-1] = pub_key[i];
    }

    string pub_key_64_s((char*) pub_key_64);
    std::cout << pub_key_64_s.size() << std::endl;
    string address_hex_org = address_to_hex(pub_key_to_address(pub_key_64_s));
    std::cout << "address_hex org:" + address_hex_org << std::endl;
    secp256k1_context *ctx1 = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    ecdsa_sign_recoverable(ctx1,msg_data,prv_key,sig_65);
    memcpy(sig_64,sig_65,64);
    string pub_key_rec = recover_pub_key_from_sig_64(sig_64,sig_65[64], msg_data);
    string pub_key_rec_hex = bytes_to_hex_string(reinterpret_cast<const uint8_t *>(pub_key_rec.data()), pub_key_rec.size());
    std::cout << "rec pub_key_hex:" + pub_key_rec_hex << std::endl;
    string address_hex = address_to_hex(pub_key_to_address(pub_key_rec));
    std::cout << "address_hex:" + address_hex << std::endl;
    auto pub_key_rec1 = recover_pub_key_from_sig_65(sig_65,msg_data);
    string pub_key_rec_hex1 = bytes_to_hex_string(reinterpret_cast<const uint8_t *>(pub_key_rec1.data()), pub_key_rec1.size());
    std::cout << "rec pub_key_hex:" + pub_key_rec_hex1 << std::endl;
    string address_hex1 = address_to_hex(pub_key_to_address(pub_key_rec1));
    std::cout << "address_hex:" + address_hex1 << std::endl;
    ASSERT_EQ(address_hex1,address_hex);
    ASSERT_EQ(pub_key_hex,pub_key_rec_hex);
    ASSERT_EQ(pub_key_hex,pub_key_rec_hex1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}