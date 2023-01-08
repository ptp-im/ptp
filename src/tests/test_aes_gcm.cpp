#include <gtest/gtest.h>

#include "secp256k1_helpers.h"
#include <secp256k1/secp256k1_ecdh.h>
#include "helpers.h"
#include "aes_encryption.h"

#include <string>
#include <vector>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/rand.h>

using namespace std;

TEST(test_random, TestIntegerOne_One) {
    unsigned char iv[16] = { 0 };
    gen_random_bytes(iv,16);
    ASSERT_TRUE(sizeof(iv) ==16);
}

TEST(test_aes_gcm_encrypt1, TestIntegerOne_One) {

    string g_data = "Across the Great Wall we can reach every corner in the world.";

    unsigned char key[32] = { 0 };
    unsigned char iv[16] = { 0 };
    unsigned char aad[16] = { 0 };
    int inLen = g_data.length();
    int encLen = 0;
    unsigned char encData[1024];

    string aadHex = "0x38f6f3c737eb1a1e187e9c673fce44d0";
    memcpy(aad, hex_to_string(aadHex.substr(2)).data(),16);
    string ivHex = "0x38f6f3c737eb1a1e187e9c673fce44d0";
    memcpy(iv, hex_to_string(ivHex.substr(2)).data(),16);
    string shared_secret_str = "0xe054f683521261beb71b95d6394eaeb9895291584aaa6e63e4b239a912fe4978";
    memcpy(key, hex_to_string(shared_secret_str.substr(2)).data(),32);

    encLen = aes_gcm_encrypt((const unsigned char*)&g_data[0], inLen,
                             key, iv,aad, encData);

    if (encLen > 0) {
        std::cout << bytes_to_hex_string(encData,encLen) << std::endl;
    }
    ASSERT_TRUE(encLen > 0);

    unsigned char decData[1024];

    int decLen = aes_gcm_decrypt(
            encData, encLen,
            key, iv,
            aad,
            decData);

    if (decLen > 0) {
        std::cout << std::string{ decData, decData + decLen } << std::endl;
    }
    ASSERT_TRUE(decLen > 0);
    string data = std::string{ decData, decData + decLen };
    ASSERT_EQ(data, g_data);
}

TEST(test_aes_gcm_encrypt, TestIntegerOne_One) {
    unsigned char seckey1[32];
    unsigned char seckey2[32];

    unsigned char pubKey11[33];
    unsigned char pubKey22[33];

    unsigned char compressed_pubkey1[33];
    unsigned char compressed_pubkey2[33];
    unsigned char shared_secret1[32];
    unsigned char shared_secret2[32];
    int return_val;
    size_t len;
    secp256k1_pubkey pubkey1;
    secp256k1_pubkey pubkey2;

    string prvKeyStr1 = "0xb5a1f6666e38d32159a0e6669a29a208d7e001071fdc85f257127ac63fffc2f1";
    string pubKeyStr33_1 = "0x032ef8556d9436a5be2fd970330cb3e4f580f8ad5abcbacf00343c74399bb14466";

    string pubKeyStr1 = "0x2ef8556d9436a5be2fd970330cb3e4f580f8ad5abcbacf00343c74399bb1446672683157d29e66e201461c01668527dd7a97c06d1b65109418b31afd57d6d873";
    string address1 = "0x60332fc033259b881fbd6658c948e07e008adfb3";

    auto prv_key1 = hex_to_string(prvKeyStr1.substr(2));
    auto pub_key33_11 = hex_to_string(pubKeyStr33_1.substr(2));
    memcpy(seckey1,prv_key1.data(),32);
    memcpy(pubKey11,pub_key33_11.data(),33);

    string prvKeyStr2 = "0x1ffe0fafed803ef0f357c8678d00089404545e8a9a9f72fb41e559ddaa9c531c";
    string pubKeyStr33_2 = "0x02f0f6796be474b77707a0ae4962c20af84bb7fc3995c91fa2ee41d2803e101069";
    string pubKeyStr2 = "0xf0f6796be474b77707a0ae4962c20af84bb7fc3995c91fa2ee41d2803e1010698217cd09b4a0753f00df3c13cd12ea39cd93c3ffc6733886347207aafbc5ac40";
    string address2 = "0xf2472d9e07c721da4bf74ddd5c587ca7f5b3ee60";
    auto prv_key2 = hex_to_string(prvKeyStr2.substr(2));
    memcpy(seckey2,prv_key2.data(),32);
    auto pub_key33_22 = hex_to_string(pubKeyStr33_2.substr(2));
    memcpy(pubKey22,pub_key33_22.data(),33);
    /* The specification in secp256k1.h states that `secp256k1_ec_pubkey_create`
     * needs a context object initialized for signing, which is why we create
     * a context with the SECP256K1_CONTEXT_SIGN flag.
     * (The docs for `secp256k1_ecdh` don't require any special context, just
     * some initialized context) */
    secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);

    return_val = secp256k1_ec_pubkey_parse(ctx,&pubkey1,pubKey11,33);
    ASSERT_TRUE(return_val);
    return_val = secp256k1_ec_pubkey_parse(ctx,&pubkey2,pubKey22,33);
    ASSERT_TRUE(return_val);
    /* Serialize pubkey1 in a compressed form (33 bytes), should always return 1 */
    len = sizeof(compressed_pubkey1);
    return_val = secp256k1_ec_pubkey_serialize(ctx, compressed_pubkey1, &len, &pubkey1, SECP256K1_EC_COMPRESSED);
    assert(return_val);
    /* Should be the same size as the size of the output, because we passed a 33 byte array. */
    assert(len == sizeof(compressed_pubkey1));

    /* Serialize pubkey2 in a compressed form (33 bytes) */
    len = sizeof(compressed_pubkey2);
    return_val = secp256k1_ec_pubkey_serialize(ctx, compressed_pubkey2, &len, &pubkey2, SECP256K1_EC_COMPRESSED);
    assert(return_val);
    /* Should be the same size as the size of the output, because we passed a 33 byte array. */
    assert(len == sizeof(compressed_pubkey2));

    /*** Creating the shared secret ***/

    /* Perform ECDH with seckey1 and pubkey2. Should never fail with a verified
     * seckey and valid pubkey */
    return_val = secp256k1_ecdh(ctx, shared_secret1, &pubkey2, seckey1, NULL, NULL);
    assert(return_val);

    /* Perform ECDH with seckey2 and pubkey1. Should never fail with a verified
     * seckey and valid pubkey */
    return_val = secp256k1_ecdh(ctx, shared_secret2, &pubkey1, seckey2, NULL, NULL);
    assert(return_val);

    /* Both parties should end up with the same shared secret */
    return_val = memcmp(shared_secret1, shared_secret2, sizeof(shared_secret1));
    assert(return_val == 0);


    /* This will clear everything from the context and free the memory */
    secp256k1_context_destroy(ctx);

    /* It's best practice to try to clear secrets from memory after using them.
     * This is done because some bugs can allow an attacker to leak memory, for
     * example through "out of bounds" array access (see Heartbleed), Or the OS
     * swapping them to disk. Hence, we overwrite the secret key buffer with zeros.
     *
     * TODO: Prevent these writes from being optimized out, as any good compiler
     * will remove any writes that aren't used. */
    memset(seckey1, 0, sizeof(seckey1));
    memset(seckey2, 0, sizeof(seckey2));
    memset(shared_secret1, 0, sizeof(shared_secret1));
    memset(shared_secret2, 0, sizeof(shared_secret2));
    ASSERT_EQ(bytes_to_hex_string(shared_secret1,32), bytes_to_hex_string(shared_secret2,32));
    string message = "test";
    string message1;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}