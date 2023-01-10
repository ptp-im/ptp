#include <gtest/gtest.h>
#include <array>
#include "random.h"
#include <secp256k1_extrakeys.h>
#include <secp256k1_schnorrsig.h>
#include <secp256k1_ecdh.h>
#include "ptp_global/Helpers.h"
#include "ptp_crypto/secp256k1_helpers.h"
#include "ptp_crypto/ptp_helpers.h"


TEST(secp256k1_ecdsa_test, TestIntegerOne_One) {
    unsigned char msg_hash[32] = {
            0x31, 0x5F, 0x5B, 0xDB, 0x76, 0xD0, 0x78, 0xC4,
            0x3B, 0x8A, 0xC0, 0x06, 0x4E, 0x4A, 0x01, 0x64,
            0x61, 0x2B, 0x1F, 0xCE, 0x77, 0xC8, 0x69, 0x34,
            0x5B, 0xFC, 0x94, 0xC7, 0x58, 0x94, 0xED, 0xD3,
    };


    unsigned char seckey[32];
    unsigned char randomize[32];
    unsigned char compressed_pubkey[33];
    unsigned char serialized_signature[64];
    size_t len;
    int is_signature_valid;
    int return_val;
    secp256k1_pubkey pubkey;
    secp256k1_ecdsa_signature sig;
    /* The specification in secp256k1.h states that `secp256k1_ec_pubkey_create` needs
     * a context object initialized for signing and `secp256k1_ecdsa_verify` needs
     * a context initialized for verification, which is why we create a context
     * for both signing and verification with the SECP256K1_CONTEXT_SIGN and
     * SECP256K1_CONTEXT_VERIFY flags. */
    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    if (!fill_random(randomize, sizeof(randomize))) {
        printf("Failed to generate randomness\n");
        exit(1);
    }
    /* Randomizing the context is recommended to protect against side-channel
     * leakage See `secp256k1_context_randomize` in secp256k1.h for more
     * information about it. This should never fail. */
    return_val = secp256k1_context_randomize(ctx, randomize);
    assert(return_val);

    /*** Key Generation ***/

    /* If the secret key is zero or out of range (bigger than secp256k1's
     * order), we try to sample a new key. Note that the probability of this
     * happening is negligible. */
    while (1) {
        if (!fill_random(seckey, sizeof(seckey))) {
            printf("Failed to generate randomness\n");
            exit(1);

        }
        if (secp256k1_ec_seckey_verify(ctx, seckey)) {
            break;
        }
    }

    /* Public key creation using a valid context with a verified secret key should never fail */
    return_val = secp256k1_ec_pubkey_create(ctx, &pubkey, seckey);
    assert(return_val);

    /* Serialize the pubkey in a compressed form(33 bytes). Should always return 1. */
    len = sizeof(compressed_pubkey);
    return_val = secp256k1_ec_pubkey_serialize(ctx, compressed_pubkey, &len, &pubkey, SECP256K1_EC_COMPRESSED);
    assert(return_val);
    /* Should be the same size as the size of the output, because we passed a 33 byte array. */
    assert(len == sizeof(compressed_pubkey));

    /*** Signing ***/

    /* Generate an ECDSA signature `noncefp` and `ndata` allows you to pass a
     * custom nonce function, passing `NULL` will use the RFC-6979 safe default.
     * Signing with a valid context, verified secret key
     * and the default nonce function should never fail. */
    return_val = secp256k1_ecdsa_sign(ctx, &sig, msg_hash, seckey, NULL, NULL);
    assert(return_val);

    /* Serialize the signature in a compact form. Should always return 1
     * according to the documentation in secp256k1.h. */
    return_val = secp256k1_ecdsa_signature_serialize_compact(ctx, serialized_signature, &sig);
    assert(return_val);


    /*** Verification ***/

    /* Deserialize the signature. This will return 0 if the signature can't be parsed correctly. */
    if (!secp256k1_ecdsa_signature_parse_compact(ctx, &sig, serialized_signature)) {
        printf("Failed parsing the signature\n");
        exit(1);

    }

    /* Deserialize the public key. This will return 0 if the public key can't be parsed correctly. */
    if (!secp256k1_ec_pubkey_parse(ctx, &pubkey, compressed_pubkey, sizeof(compressed_pubkey))) {
        printf("Failed parsing the public key\n");
        exit(1);
    }

    /* Verify a signature. This will return 1 if it's valid and 0 if it's not. */
    is_signature_valid = secp256k1_ecdsa_verify(ctx, &sig, msg_hash, &pubkey);

    printf("Is the signature valid? %s\n", is_signature_valid ? "true" : "false");
    printf("Secret Key: ");
    print_hex(seckey, sizeof(seckey));
    printf("Public Key: ");
    print_hex(compressed_pubkey, sizeof(compressed_pubkey));
    printf("Signature: ");
    print_hex(serialized_signature, sizeof(serialized_signature));

    /* This will clear everything from the context and free the memory */
    secp256k1_context_destroy(ctx);

    /* It's best practice to try to clear secrets from memory after using them.
     * This is done because some bugs can allow an attacker to leak memory, for
     * example through "out of bounds" array access (see Heartbleed), Or the OS
     * swapping them to disk. Hence, we overwrite the secret key buffer with zeros.
     *
     * TODO: Prevent these writes from being optimized out, as any good compiler
     * will remove any writes that aren't used. */
    memset(seckey, 0, sizeof(seckey));
    ASSERT_EQ(1, 1);
}



TEST(secp256k1_schnorrsig_test, TestIntegerOne_One)
{
    unsigned char msg[] = "Hello World!";
    unsigned char msg_hash[32];
    unsigned char tag[] = "my_fancy_protocol";
    unsigned char seckey[32];
    unsigned char randomize[32];
    unsigned char auxiliary_rand[32];
    unsigned char serialized_pubkey[32];
    unsigned char signature[64];
    int is_signature_valid;
    int return_val;
    secp256k1_xonly_pubkey pubkey;
    secp256k1_keypair keypair;
    /* The specification in secp256k1_extrakeys.h states that `secp256k1_keypair_create`
     * needs a context object initialized for signing. And in secp256k1_schnorrsig.h
     * they state that `secp256k1_schnorrsig_verify` needs a context initialized for
     * verification, which is why we create a context for both signing and verification
     * with the SECP256K1_CONTEXT_SIGN and SECP256K1_CONTEXT_VERIFY flags. */
    secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    if (!fill_random(randomize, sizeof(randomize))) {
        printf("Failed to generate randomness\n");
        exit(1);
    }
    /* Randomizing the context is recommended to protect against side-channel
     * leakage See `secp256k1_context_randomize` in secp256k1.h for more
     * information about it. This should never fail. */
    return_val = secp256k1_context_randomize(ctx, randomize);
    assert(return_val);

    /*** Key Generation ***/

    /* If the secret key is zero or out of range (bigger than secp256k1's
     * order), we try to sample a new key. Note that the probability of this
     * happening is negligible. */
    while (1) {
        if (!fill_random(seckey, sizeof(seckey))) {
            printf("Failed to generate randomness\n");
            exit(1);
        }
        /* Try to create a keypair with a valid context, it should only fail if
         * the secret key is zero or out of range. */
        if (secp256k1_keypair_create(ctx, &keypair, seckey)) {
            break;
        }
    }

    /* Extract the X-only public key from the keypair. We pass NULL for
     * `pk_parity` as the parity isn't needed for signing or verification.
     * `secp256k1_keypair_xonly_pub` supports returning the parity for
     * other use cases such as tests or verifying Taproot tweaks.
     * This should never fail with a valid context and public key. */
    return_val = secp256k1_keypair_xonly_pub(ctx, &pubkey, NULL, &keypair);
    assert(return_val);

    /* Serialize the public key. Should always return 1 for a valid public key. */
    return_val = secp256k1_xonly_pubkey_serialize(ctx, serialized_pubkey, &pubkey);
    assert(return_val);

    /*** Signing ***/

    /* Instead of signing (possibly very long) messages directly, we sign a
     * 32-byte hash of the message in this example.
     *
     * We use secp256k1_tagged_sha256 to create this hash. This function expects
     * a context-specific "tag", which restricts the context in which the signed
     * messages should be considered valid. For example, if protocol A mandates
     * to use the tag "my_fancy_protocol" and protocol B mandates to use the tag
     * "my_boring_protocol", then signed messages from protocol A will never be
     * valid in protocol B (and vice versa), even if keys are reused across
     * protocols. This implements "domain separation", which is considered good
     * practice. It avoids attacks in which users are tricked into signing a
     * message that has intended consequences in the intended context (e.g.,
     * protocol A) but would have unintended consequences if it were valid in
     * some other context (e.g., protocol B). */
    return_val = secp256k1_tagged_sha256(ctx, msg_hash, tag, sizeof(tag), msg, sizeof(msg));
    assert(return_val);

    /* Generate 32 bytes of randomness to use with BIP-340 schnorr signing. */
    if (!fill_random(auxiliary_rand, sizeof(auxiliary_rand))) {
        printf("Failed to generate randomness\n");
        exit(1);
    }

    /* Generate a Schnorr signature.
     *
     * We use the secp256k1_schnorrsig_sign32 function that provides a simple
     * interface for signing 32-byte messages (which in our case is a hash of
     * the actual message). BIP-340 recommends passing 32 bytes of randomness
     * to the signing function to improve security against side-channel attacks.
     * Signing with a valid context, a 32-byte message, a verified keypair, and
     * any 32 bytes of auxiliary random data should never fail. */
    return_val = secp256k1_schnorrsig_sign32(ctx, signature, msg_hash, &keypair, auxiliary_rand);
    assert(return_val);

    /*** Verification ***/

    /* Deserialize the public key. This will return 0 if the public key can't
     * be parsed correctly */
    if (!secp256k1_xonly_pubkey_parse(ctx, &pubkey, serialized_pubkey)) {
        printf("Failed parsing the public key\n");
        exit(1);
    }

    /* Compute the tagged hash on the received messages using the same tag as the signer. */
    return_val = secp256k1_tagged_sha256(ctx, msg_hash, tag, sizeof(tag), msg, sizeof(msg));
    assert(return_val);

    /* Verify a signature. This will return 1 if it's valid and 0 if it's not. */
    is_signature_valid = secp256k1_schnorrsig_verify(ctx, signature, msg_hash, 32, &pubkey);


    printf("Is the signature valid? %s\n", is_signature_valid ? "true" : "false");
    printf("Secret Key: ");
    print_hex(seckey, sizeof(seckey));
    printf("Public Key: ");
    print_hex(serialized_pubkey, sizeof(serialized_pubkey));
    printf("Signature: ");
    print_hex(signature, sizeof(signature));

    /* This will clear everything from the context and free the memory */
    secp256k1_context_destroy(ctx);

    /* It's best practice to try to clear secrets from memory after using them.
     * This is done because some bugs can allow an attacker to leak memory, for
     * example through "out of bounds" array access (see Heartbleed), Or the OS
     * swapping them to disk. Hence, we overwrite the secret key buffer with zeros.
     *
     * TODO: Prevent these writes from being optimized out, as any good compiler
     * will remove any writes that aren't used. */
    memset(seckey, 0, sizeof(seckey));
    ASSERT_EQ(1, 1);
}


TEST(secp256k1_ecdh_test, TestIntegerOne_One)
{
    unsigned char seckey1[32];
    unsigned char seckey2[32];
    unsigned char compressed_pubkey1[33];
    unsigned char compressed_pubkey2[33];
    unsigned char shared_secret1[32];
    unsigned char shared_secret2[32];
    unsigned char randomize[32];
    int return_val;
    size_t len;
    secp256k1_pubkey pubkey1;
    secp256k1_pubkey pubkey2;

    /* The specification in secp256k1.h states that `secp256k1_ec_pubkey_create`
     * needs a context object initialized for signing, which is why we create
     * a context with the SECP256K1_CONTEXT_SIGN flag.
     * (The docs for `secp256k1_ecdh` don't require any special context, just
     * some initialized context) */
    secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
    if (!fill_random(randomize, sizeof(randomize))) {
        printf("Failed to generate randomness\n");
        exit(1);
    }
    /* Randomizing the context is recommended to protect against side-channel
     * leakage See `secp256k1_context_randomize` in secp256k1.h for more
     * information about it. This should never fail. */
    return_val = secp256k1_context_randomize(ctx, randomize);
    assert(return_val);

    /*** Key Generation ***/

    /* If the secret key is zero or out of range (bigger than secp256k1's
     * order), we try to sample a new key. Note that the probability of this
     * happening is negligible. */
    while (1) {
        if (!fill_random(seckey1, sizeof(seckey1)) || !fill_random(seckey2, sizeof(seckey2))) {
            printf("Failed to generate randomness\n");
            exit(1);
        }
        if (secp256k1_ec_seckey_verify(ctx, seckey1) && secp256k1_ec_seckey_verify(ctx, seckey2)) {
            break;
        }
    }

    /* Public key creation using a valid context with a verified secret key should never fail */
    return_val = secp256k1_ec_pubkey_create(ctx, &pubkey1, seckey1);
    assert(return_val);
    return_val = secp256k1_ec_pubkey_create(ctx, &pubkey2, seckey2);
    assert(return_val);

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

    printf("Secret Key1: ");
    print_hex(seckey1, sizeof(seckey1));
    printf("Compressed Pubkey1: ");
    print_hex(compressed_pubkey1, sizeof(compressed_pubkey1));
    printf("\nSecret Key2: ");
    print_hex(seckey2, sizeof(seckey2));
    printf("Compressed Pubkey2: ");
    print_hex(compressed_pubkey2, sizeof(compressed_pubkey2));
    printf("\nShared Secret: \n");
    print_hex(shared_secret1, sizeof(shared_secret1));
    print_hex(shared_secret2, sizeof(shared_secret2));

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
}

TEST(secp256k1_sign, TestIntegerOne_One) {
    unsigned char sig_64[64];
    unsigned char sig_65[65];
    unsigned char pub_key_33[33];
    unsigned char pub_key_64[64];

    string message = "test_message";
    string msg_data = format_eth_msg_data(message);

    string prvKeyStr = "0xb5a1f6666e38d32159a0e6669a29a208d7e001071fdc85f257127ac63fffc2f1";
    string pubKeyStr = "0x2ef8556d9436a5be2fd970330cb3e4f580f8ad5abcbacf00343c74399bb1446672683157d29e66e201461c01668527dd7a97c06d1b65109418b31afd57d6d873";
    string address = "0x60332fc033259b881fbd6658c948e07e008adfb3";
    printf("pubKeyStr1:   %s\n",pubKeyStr.c_str());
    auto prv_key = hex_to_string(prvKeyStr.substr(2));
    auto pub_key = hex_to_string(pubKeyStr.substr(2));

    string pub_key_hex = bytes_to_hex_string(reinterpret_cast<const uint8_t *>(pub_key.data()), pub_key.size());
    ASSERT_EQ(pubKeyStr,pub_key_hex);

    string address_hex_org = address_to_hex(pub_key_to_address(pub_key));
    ASSERT_EQ(address_hex_org,address);
    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    unsigned char* prv_key_bytes = (unsigned char*) prv_key.c_str();

    ecdsa_sign_recoverable(ctx,msg_data,prv_key_bytes,sig_65);
    memcpy(sig_64,sig_65,64);
    string pub_key_rec = recover_pub_key_from_sig_64(sig_64,sig_65[64], msg_data);
    string pub_key_rec_hex = bytes_to_hex_string(reinterpret_cast<const uint8_t *>(pub_key_rec.data()), pub_key_rec.size());
    ASSERT_EQ(pub_key_rec_hex,pub_key_hex);

    string address_hex = address_to_hex(pub_key_to_address(pub_key_rec));
    ASSERT_EQ(address_hex,address_hex_org);;
    string address11;
    auto ret1 = recover_pub_key_from_sig_65(sig_65,msg_data, pub_key_33, address11);
    ASSERT_EQ(address11,address_hex);;

    ASSERT_TRUE(ret1);

}

TEST(secp256k1_ecdh_test1, TestIntegerOne_One)
{
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

    printf("Secret Key1: ");
    print_hex(seckey1, sizeof(seckey1));
    printf("Compressed Pubkey1: ");
    print_hex(compressed_pubkey1, sizeof(compressed_pubkey1));
    printf("\nSecret Key2: ");
    print_hex(seckey2, sizeof(seckey2));
    printf("Compressed Pubkey2: ");
    print_hex(compressed_pubkey2, sizeof(compressed_pubkey2));
    printf("\nShared Secret: \n");
    print_hex(shared_secret1, sizeof(shared_secret1));
    print_hex(shared_secret2, sizeof(shared_secret2));

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
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}