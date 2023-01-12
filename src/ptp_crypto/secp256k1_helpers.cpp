#include <array>
#include "secp256k1/secp256k1_ecdh.h"
#include "secp256k1/secp256k1_recovery.h"
#include "secp256k1_helpers.h"
#include "keccak.h"
#include "crypto_helpers.h"

using namespace std;

string format_sign_msg_data(const string &message,SignMsgType signMsgType){
    if(signMsgType == SignMsgType_ptp){
        return string("\x19") + PTP_Signed_Message + to_string(message.size()) + message;
    }else if(signMsgType == SignMsgType_ptp_group){
        return string("\x19") + PTP_GROUP_Signed_Message + to_string(message.size()) + message;
    }else{
        return string("\x19") + ETH_Signed_Message + to_string(message.size()) + message;
    }
}

string recover_address_from_sig_hex(string sig_hex, string msg)
{
    string _sig = hex_to_string(sig_hex.substr(2)); // strip 0x
    return recover_address_from_sig(_sig,msg);
}

string pub_key_to_address(string pubkey_64){
    array<uint8_t, 32> pub_key_hash;
    Keccak256::getHash(reinterpret_cast<const uint8_t *>(pubkey_64.data()), 64, pub_key_hash.data());
    string address_hash = string(pub_key_hash.begin(), pub_key_hash.end()).substr(12);
    return address_hash;
}

string recover_address_from_sig(string _sig, string msg){
    string pub_key_64 = recover_pub_key_from_sig(_sig, msg);
    return pub_key_to_address(pub_key_64);
}

string recover_pub_key_from_sig_hex(string sig_hex, string msg)
{
    string _sig = hex_to_string(sig_hex.substr(2)); // strip 0x
    return recover_pub_key_from_sig(_sig,msg);
}

void gen_ec_key_pair(secp256k1_context * ctx,unsigned char * prv_key, unsigned char * pub_key){
    unsigned char randomize[32];
    unsigned char prv_key_[32];
    unsigned char pub_key_[65];
    secp256k1_pubkey pubkey;
    auto ret = secp256k1_context_randomize(ctx, randomize);
    if(!ret){
        return;
    }
    auto ret1 = secp256k1_ec_pubkey_create(ctx, &pubkey, prv_key_);
    if(!ret1){
        return;
    }
    auto len = sizeof(pub_key_);
    secp256k1_ec_pubkey_serialize(ctx, pub_key_, &len, &pubkey, SECP256K1_EC_UNCOMPRESSED);
//    pub_key_to_hex(pub_key_);
    memcpy(pub_key,pub_key_,65);
    memcpy(prv_key,prv_key_,32);
    secp256k1_context_destroy(ctx);
}

string recover_pub_key_from_sig_64(unsigned char * sig,int rec_id, string msg_data)
{
    int v = rec_id;
    if (v < 27)  v += 27;
    if (v > 3) v -= 27;
    array<uint8_t, 32> msg_hash;
    msg_hash = keccak_256_hash(msg_data);
    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    secp256k1_ecdsa_recoverable_signature rawSig;

    if (!secp256k1_ecdsa_recoverable_signature_parse_compact(ctx, &rawSig, sig, v)) {
//        log_error("parse sign compact error");
        return "";
    }
    secp256k1_pubkey rawPubkey;
    if (!secp256k1_ecdsa_recover(ctx, &rawPubkey, &rawSig,
                                 msg_hash.data()))
    {
//        log_error("recover sign error");
        return "";
    }
    array<uint8_t, 65> pubkey;
    size_t biglen = 65;
    secp256k1_ec_pubkey_serialize(ctx, pubkey.data(), &biglen, &rawPubkey, SECP256K1_EC_UNCOMPRESSED);
//    pub_key_to_hex(pubkey.data());
    string pubkey_64 = string(pubkey.begin(), pubkey.end()).substr(1);
    secp256k1_context_destroy(ctx);
    return pubkey_64;
}

string recover_pub_key_from_sig_65(unsigned char * sig_65, string msg_data)
{
    unsigned char sig_64[64];
    memcpy(sig_64,sig_65,64);
    int rec_id = sig_65[64];
    return recover_pub_key_from_sig_64(sig_64,rec_id,msg_data);
}

bool recover_pub_key_from_sig_65(unsigned char * sig_65,const string &msg_data,unsigned char * pub_key_33,string& address)
{
    unsigned char sig_64[64];
    unsigned char pub_key_65[65];
    memcpy(sig_64,sig_65,64);
    int rec_id = sig_65[64];

    int v = rec_id;
    if (v < 27)  v += 27;
    if (v > 3) v -= 27;
    array<uint8_t, 32> msg_hash;
    msg_hash = keccak_256_hash(msg_data);
    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    secp256k1_ecdsa_recoverable_signature rawSig;

    if (!secp256k1_ecdsa_recoverable_signature_parse_compact(ctx, &rawSig, sig_64, v)) {
        return false;
    }
    secp256k1_pubkey rawPubkey;
    if (!secp256k1_ecdsa_recover(ctx,
                                 &rawPubkey,
                                 &rawSig,
                                 msg_hash.data()))
    {
        return false;
    }
    size_t len = 65;
    auto ret = secp256k1_ec_pubkey_serialize(ctx, pub_key_65, &len, &rawPubkey, SECP256K1_EC_UNCOMPRESSED);
    if(!ret){
        return false;
    }
    size_t len1 = 33;
    ret = secp256k1_ec_pubkey_serialize(ctx, pub_key_33, &len1, &rawPubkey, SECP256K1_EC_COMPRESSED);
    if(!ret){
        return false;
    }
    unsigned char pub_key_64[64];

    for (size_t i = 1; i < 65; ++i){
        pub_key_64[i - 1] = pub_key_65[i];
    }
    string pub_key_64_hex = bytes_to_hex_string(pub_key_64, 64);
    address = address_to_hex(pub_key_to_address(hex_to_string(pub_key_64_hex.substr(2))));

    secp256k1_context_destroy(ctx);
    return ret;
}

bool recover_pub_key_and_address_from_sig(unsigned char * sig_65,const string &msg_data,unsigned char * pub_key,string& address){
    return recover_pub_key_from_sig_65(sig_65,msg_data,pub_key,address);
}

string recover_pub_key_from_sig(string sig, string msg_data)
{
    if (sig.size() != 65) {
//        log_error("error sig len");
        return "";
    }
    auto _sig = sig.substr(0, 64);
    return recover_pub_key_from_sig_64((unsigned char *) _sig.data(),sig[64],msg_data);
}

void ecdsa_sign_recoverable(secp256k1_context *ctx,string msg_data,unsigned char * prv_key,unsigned char *output65){
    array<uint8_t, 32> msg_hash = keccak_256_hash(msg_data);
    unsigned char sig_65[65];
    unsigned char sig_64[64];
    int rec_id;
    secp256k1_ecdsa_recoverable_signature sig;
    secp256k1_ecdsa_sign_recoverable(ctx, &sig, msg_hash.data(), prv_key, nullptr, nullptr);
    secp256k1_ecdsa_recoverable_signature_serialize_compact(ctx,sig_64,&rec_id,&sig);
    memcpy(sig_65,sig_64,64);
    sig_65[64] = rec_id;
    memcpy(output65,sig_65,65);
    secp256k1_context_destroy(ctx);
}

bool ecdh_create_share_key(secp256k1_pubkey *pub_key,string &prv_key,unsigned char *shared_key){
    secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
    unsigned char prv_key_32[32];
    string prv_key1 = hex_to_string(prv_key.substr(2));
    memcpy(prv_key_32,prv_key1.data(),32);
    auto ret = secp256k1_ecdh(ctx, shared_key, pub_key, prv_key_32, NULL, NULL);
    secp256k1_context_destroy(ctx);
    return ret;
}

bool ecdh_create_share_key(secp256k1_pubkey *pub_key,unsigned char *prv_key,unsigned char *shared_key){
    secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
    auto ret = secp256k1_ecdh(ctx, shared_key, pub_key, prv_key, NULL, NULL);
    secp256k1_context_destroy(ctx);
    return ret;
}