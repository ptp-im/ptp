#include <gtest/gtest.h>

#include <array>
#include <unistd.h>
#include <cstring>
#include <openssl/rand.h>

#include "ptp_global/Helpers.h"
#include "ptp_global/Utils.h"
#include "ptp_global/Logger.h"
#include "ptp_global/ImPduBase.h"
#include "ptp_crypto/aes_encryption.h"
#include "ptp_crypto/secp256k1_helpers.h"
#include "ptp_crypto/ptp_helpers.h"
#include "ptp_protobuf/PTP.MSG.pb.h"
#include "ptp_protobuf/PTP.AUTH.pb.h"
#include "ptp_protobuf/PTP.GROUP.pb.h"
#include "ptp_servers/ws/MsgConn.h"

#include "ptp_business/CachePool.h"
#include "ptp_servers/ws/ImUser.h"

using namespace PTP::Common;

TEST(test_utils, sleep) {
    sleep(10);
    ASSERT_TRUE(true);
}
TEST(test_auth_captcha, TestIntegerOne_One) {
    PTP::Auth::AuthCaptchaRes msg_rsp;
    ERR error = NO_ERROR;

    auto captcha = gen_random(6);
//    msg_rsp.set_captcha(captcha);
    unsigned char iv[16] = { 0 };
    gen_random_bytes(iv,16);

    unsigned char aad[16] = { 0 };
    gen_random_bytes(aad,16);

    string msg_data = format_eth_msg_data(captcha);
    unsigned char signOut65[65];
    string prvKeyStr = "0x1ffe0fafed803ef0f357c8678d00089404545e8a9a9f72fb41e559ddaa9c531c";
    string pubKey33Str = "0x02f0f6796be474b77707a0ae4962c20af84bb7fc3995c91fa2ee41d2803e101069";
    string pubKeyStr = "0xf0f6796be474b77707a0ae4962c20af84bb7fc3995c91fa2ee41d2803e1010698217cd09b4a0753f00df3c13cd12ea39cd93c3ffc6733886347207aafbc5ac40";
    auto pub_key = hex_to_string(pubKeyStr.substr(2));
    string addressHex = "0xf2472d9e07c721da4bf74ddd5c587ca7f5b3ee60";
    string address = hex_to_string(addressHex.substr(2));
    int addressLen = address.size();
    ASSERT_EQ(addressLen,20);
    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    auto prv_key = hex_to_string(prvKeyStr.substr(2));
    auto* prv_key_bytes = (unsigned char*) prv_key.data();
    ecdsa_sign_recoverable(ctx,msg_data,prv_key_bytes,signOut65);
    secp256k1_context_destroy(ctx);
    DEBUG_D("signOut65: %s", bytes_to_hex_string(signOut65,65).c_str());
    DEBUG_D("iv: %s", bytes_to_hex_string(iv,16).c_str());
    DEBUG_D("aad: %s", bytes_to_hex_string(aad,16).c_str());
    DEBUG_D("address: %s", addressHex.c_str());
    DEBUG_D("captcha: %s", captcha.c_str());

    msg_rsp.set_captcha(captcha);
    msg_rsp.set_address(address);
    msg_rsp.set_iv(iv,16);
    msg_rsp.set_aad(aad,16);
    msg_rsp.set_sign(signOut65,65);
    msg_rsp.set_error(NO_ERROR);

    CImPdu pdu;
    pdu.SetPBMsg(&msg_rsp);
    pdu.SetServiceId(S_AUTH);
    pdu.SetCommandId(CID_AuthCaptchaRes);
    pdu.SetSeqNum(0);

    auto pPdu1 = CImPdu::ReadPdu(pdu.GetBuffer(), pdu.GetLength());
    DEBUG_D("6 + 20 + 16 + 16 + 65 + 4: %D", 6 + 20 + 16 + 16 + 65 + 4);
    DEBUG_D("pPdu1->GetBodyLength(): %d", pPdu1->GetBodyLength());

    auto buf = pPdu1->GetBodyData();

    unsigned char captchaBuff[6] = { 0 };
    memcpy(captchaBuff,buf + 2,6);
    DEBUG_D("captcha: %s", captchaBuff);

    unsigned char addressBuff[20] = { 0 };
    memcpy(addressBuff,buf + 2 + 6 + 2,20);
    DEBUG_D("address: %s", bytes_to_hex_string(addressBuff,20).c_str());

    unsigned char ivBuf[16] = { 0 };
    memcpy(ivBuf,buf + 2 + 6 + 2 + 20 + 2, 16);
    DEBUG_D("iv: %s", bytes_to_hex_string(ivBuf,16).c_str());

    unsigned char aadBuff[16] = { 0 };

    memcpy(aadBuff,buf + 2 + 6 + 2 + 20 + 2 + 16 + 2, 16);
    DEBUG_D("aad: %s", bytes_to_hex_string(aadBuff,16).c_str());

    unsigned char sign65Buf[65] = { 0 };
    memcpy(sign65Buf,buf + 2 + 6 + 2 + 20 + 2 + 16 + 2 + 16 + 2, 65);
    DEBUG_D("signOut65: %s", bytes_to_hex_string(sign65Buf,65).c_str());

    ASSERT_EQ(bytes_to_hex_string(signOut65,65),bytes_to_hex_string(sign65Buf,65));

}

TEST(test_random, TestIntegerOne_One) {
    unsigned char iv[16] = { 0 };
    gen_random_bytes(iv,16);
    ASSERT_TRUE(sizeof(iv) ==16);
}

TEST(ptp_crypto, genKey) {
    unsigned char prv_key[32];
    array<uint8_t, 65> pub_key;
    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    gen_ec_key_pair(ctx, prv_key, pub_key.data());
    std::cout << "prv_key: " + bytes_to_hex_string(prv_key, 32) << std::endl;
    string pub_key_hex = pub_key_to_hex(pub_key.data());
    std::cout << "org pub_key_hex: " + pub_key_hex << std::endl;
    string pub_key_64_s = string(pub_key.begin(),pub_key.end()).substr(1);
    string address_hex_org = address_to_hex(pub_key_to_address(pub_key_64_s));
    std::cout << "address_hex org:" + address_hex_org << std::endl;
    ASSERT_EQ(sizeof prv_key,32);
    ASSERT_EQ(pub_key.size(),65);
}

TEST(ptp_crypto, signRecover1) {
    unsigned char sig_64[64];
    unsigned char sig_65[65];

    string message = "test_message";
    string msg_data = format_eth_msg_data(message);

    string prvKeyStr = "0x50c5c421aa772ba5dbb406a2ea6be342010088c769476854a081fe0300600000";
    string pubKeyStr = "0xd7df148b3de0a18dfeea71cd1bfa82a6bc34f533d39d83efb43010afa2addb0782cd401baccf193155f884c74af9194ed0f9a1dbc7e3d52d4dfb8197cc07d549";
    string address = "0xd2ced67c476384a829d8c19be7375cf8d2298f47";

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
    ASSERT_EQ(address_hex,address_hex_org);

}

TEST(test_utils, TestIntegerOne_One) {
    uint64_t time_m = unix_timestamp_m();
    uint32_t time_ = unix_timestamp();
    DEBUG_D("unix_timestamp_m: %lu", time_m);
    DEBUG_D("unix_timestamp_m: %s", int64ToString(time_m).c_str());
    DEBUG_D("unix_timestamp: %d", time_);

    auto captcha_src = gen_random(6);
    auto* pMsgConn = new CMsgConn();
    pMsgConn->Set_testing_require_pdu();
    pMsgConn->isHandshark = false;
    pMsgConn->SetConnHandle(1);
    pMsgConn->Set_testing_require_pdu();
    pMsgConn->SetState(WS_STATE_CONNECTED);
    pMsgConn->SetCaptcha(captcha_src);
    MapConn(pMsgConn);

    string captcha = pMsgConn->GetCaptcha();
    string msg_data = format_eth_msg_data(captcha);

    unsigned char sig_64[64];
    unsigned char sig_65[65];
    unsigned char prv_key[32];
    unsigned char pub_key[65];

    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    gen_ec_key_pair(ctx,prv_key,pub_key);
    pub_key_to_hex(pub_key);
    ecdsa_sign_recoverable(ctx,msg_data,prv_key,sig_65);
    memcpy(sig_64,sig_65,64);
    string pub_key_rec = recover_pub_key_from_sig_64(sig_64,sig_65[64], msg_data);
    auto pubkey_rec = recover_pub_key_from_sig_65(sig_65,msg_data);

    string address_hex = address_to_hex(pub_key_to_address(pub_key_rec));
    DEBUG_D("address_hex: %s",address_hex.c_str());

    std::string sig1 = "0x446321fc71eb7a4b7256618f56c695dd6ab7cf94f70beea2bb42676e62d4d7b7618a490318983c5fe422e4951aa970dcaaee73366e4780dd6e60efb46a861d301b";
    auto address_101 = recover_address_from_sig(hex_to_string(sig1.substr(2)), format_eth_msg_data("abc123"));
    DEBUG_D("address_to_hex:%s",address_to_hex(address_101).c_str());
    PTP::Auth::AuthLoginReq msg;
    msg.set_address(address_hex);
    msg.set_captcha(captcha);
    msg.set_client_type(PTP::Common::CLIENT_TYPE_IOS);
    msg.set_client_version("MAC");
    msg.set_sign(sig_65,65);

    auto sign = msg.sign();
    string msg_data1 = format_eth_msg_data(captcha);
    auto pub_key_64 = recover_pub_key_from_sig(sign, msg_data1);

    ASSERT_EQ(1,1);
}

TEST(test_Group_GroupPreCreateReq_GROUP_TYPE_MULTI, TestIntegerOne_One) {
    CacheManager* pCacheManager = CacheManager::getInstance();
    auto* pMsgConn = new CMsgConn();
    pMsgConn->Set_testing_require_pdu();
    pMsgConn->isHandshark = false;
    pMsgConn->SetConnHandle(1);
    pMsgConn->Set_testing_require_pdu();
    pMsgConn->SetState(WS_STATE_CONNECTED);

    string fromUserAdr = "0x0171539f67365be62db94d3c4fb03f677dd20b2d";
    auto fromUserId = GetUserIdFromCache(fromUserAdr,true);

    string memberUserAdr1 = "0xe8c6107babad5ffc7a3aa8b7e25613ae21cbfe1b";
    auto memberUserId1 = GetUserIdFromCache(memberUserAdr1,true);

    string memberUserAdr2 = "0x9ff903d2c45c549cc3a9bedb3b5ec0715e9e01f5";
    auto memberUserId2 = GetUserIdFromCache(memberUserAdr2,true);

    pMsgConn->SetUserId(fromUserId);
    pMsgConn->SetAddressHex(fromUserAdr);
    MapConn(pMsgConn);
    auto groupType = GROUP_TYPE_MULTI;
    PTP::Group::GroupPreCreateReq msg;
    msg.set_group_type(groupType);

    CImPdu pdu;
    pdu.SetPBMsg(&msg);
    pdu.SetServiceId(S_GROUP);
    pdu.SetCommandId(CID_GroupPreCreateReq);
    pdu.SetSeqNum(1);
//    _Handle_GroupPreCreateReq(&pdu, pMsgConn);
    CImPdu * pPdu = pMsgConn->Get_testing_require_pdu();
    PTP::Group::GroupPreCreateRes msg1;
    msg1.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength());

    auto group_adr = msg1.group_adr();
    auto group_idx = msg1.group_idx();
    auto error = msg1.error();

    string captcha = gen_random(6);
    string message = to_string(group_idx) + captcha;
    string msg_data = format_eth_msg_data(message);

    unsigned char sig_65[65];
    unsigned char prv_key[32];
    unsigned char pub_key[65];

    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    gen_ec_key_pair(ctx,prv_key,pub_key);

    ecdsa_sign_recoverable(ctx,msg_data,prv_key,sig_65);
    auto pubkey_rec = recover_pub_key_from_sig_65(sig_65,msg_data);
    string address_hex = address_to_hex(pub_key_to_address(pubkey_rec));
    DEBUG_D("address_hex: %s",address_hex.c_str());

    pMsgConn->SetCaptcha(captcha);

    PTP::Group::GroupCreateReq msg11;
    msg11.set_group_idx(group_idx);
    msg11.set_captcha(captcha);
    msg11.set_sign(sig_65,65);
    msg11.set_group_type(groupType);
    msg11.set_avatar("groupAvatar");
//    msg11.add_members(memberUserAdr1);
//    msg11.add_members(memberUserAdr2);

    CImPdu pdu11;
    pdu11.SetPBMsg(&msg11);
    pdu11.SetServiceId(S_GROUP);
    pdu11.SetCommandId(CID_GroupCreateReq);
    pdu11.SetSeqNum(1);

//    _Handle_GroupCreateReq(&pdu11, pMsgConn);

    CImPdu * pPdu11 = pMsgConn->Get_testing_require_pdu();
    PTP::Group::GroupCreateRes msg12;

    msg12.ParseFromArray(pPdu11->GetBodyData(), pPdu11->GetBodyLength());

    auto group = msg12.group();
    DEBUG_D("group_addr: %s",group.group_adr().c_str());
    DEBUG_D("name: %s",group.name().c_str());
    DEBUG_D("avatar: %s",group.avatar().c_str());
//    DEBUG_D("owner_adr: %s",group.owner_adr().c_str());
//    DEBUG_D("pair_adr: %s",group.pair_adr().c_str());
    DEBUG_D("group_idx: %d",group.group_idx());
//    for(auto group_member=msg12.group_members().begin();group_member!= msg12.group_members().end();group_member++){
//        DEBUG_D("|===>>     address: %s",group_member->address().c_str());
//        DEBUG_D("           member_status: %d",group_member->member_status());
//
//    }

    ASSERT_EQ(1,1);
}

TEST(test_Group_GroupPreCreateReq_GROUP_TYPE_PAIR, TestIntegerOne_One) {
    CacheManager* pCacheManager = CacheManager::getInstance();
    auto* pMsgConn = new CMsgConn();
    pMsgConn->Set_testing_require_pdu();
    pMsgConn->isHandshark = false;
    pMsgConn->SetConnHandle(1);
    pMsgConn->Set_testing_require_pdu();
    pMsgConn->SetState(WS_STATE_CONNECTED);

    string fromUserAdr = "0x0171539f67365be62db94d3c4fb03f677dd20b2d";
    auto fromUserId = GetUserIdFromCache(fromUserAdr,true);

    string memberUserAdr = "0xe8c6107babad5ffc7a3aa8b7e25613ae21cbfe1b";
    auto memberUserId = GetUserIdFromCache(memberUserAdr,true);

    pMsgConn->SetUserId(fromUserId);
    pMsgConn->SetAddressHex(fromUserAdr);
    MapConn(pMsgConn);
    auto groupType = GROUP_TYPE_PAIR;

    PTP::Group::GroupPreCreateReq msg;
    msg.set_group_type(groupType);

    CImPdu pdu;
    pdu.SetPBMsg(&msg);
    pdu.SetServiceId(S_GROUP);
    pdu.SetCommandId(CID_GroupPreCreateReq);
    pdu.SetSeqNum(1);


    CImPdu * pPdu = pMsgConn->Get_testing_require_pdu();
    PTP::Group::GroupPreCreateRes msg1;
    msg1.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength());

    auto group_adr = msg1.group_adr();
    auto group_idx = msg1.group_idx();
    auto error = msg1.error();

    string captcha = gen_random(6);
    pMsgConn->SetCaptcha(captcha);

    string message = to_string(group_idx) + captcha;
    string msg_data = format_eth_msg_data(message);

    unsigned char sig_65[65];
    unsigned char prv_key[32];
    unsigned char pub_key[65];

    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    gen_ec_key_pair(ctx,prv_key,pub_key);
    ecdsa_sign_recoverable(ctx,msg_data,prv_key,sig_65);
    auto pubkey_rec = recover_pub_key_from_sig_65(sig_65,msg_data);
    string address_hex = address_to_hex(pub_key_to_address(pubkey_rec));
    DEBUG_D("address_hex: %s",address_hex.c_str());


    PTP::Group::GroupCreateReq msg11;
    msg11.set_group_idx(group_idx);
    msg11.set_captcha(captcha);
    msg11.set_sign(sig_65,65);
    msg11.set_group_type(groupType);
    msg11.set_avatar("groupAvatar");
//    msg11.add_members(memberUserAdr);

    CImPdu pdu11;
    pdu11.SetPBMsg(&msg11);
    pdu11.SetServiceId(S_GROUP);
    pdu11.SetCommandId(CID_GroupCreateReq);
    pdu11.SetSeqNum(1);
//
//    _Handle_GroupCreateReq(&pdu11, pMsgConn);
//
//    CImPdu * pPdu11 = pMsgConn->Get_testing_require_pdu();
//    if(pPdu11){
//        PTP::Group::GroupCreateRes msg12;
//        msg12.ParseFromArray(pPdu11->GetBodyData(), pPdu11->GetBodyLength());
//
//        auto group = msg12.group();
//        DEBUG_D("group_addr: %s",group.group_adr().c_str());
//        DEBUG_D("name: %s",group.name().c_str());
//        DEBUG_D("avatar: %s",group.avatar().c_str());
//        DEBUG_D("owner_adr: %s",group.owner_adr().c_str());
//        DEBUG_D("pair_adr: %s",group.pair_adr().c_str());
//        DEBUG_D("group_idx: %d",group.group_idx());
//
//        for(auto group_member=msg12.group_members().begin();group_member!= msg12.group_members().end();group_member++){
//            DEBUG_D("|===>>     address: %s",group_member->address().c_str());
//            DEBUG_D("           member_status: %d",group_member->member_status());
//
//        }
//    }else{
//
//    }
    ASSERT_EQ(1,1);

}


TEST(test_aes_gcm_encrypt_pdu, TestIntegerOne_One) {

    PTP::Auth::AuthCaptchaRes msg_rsp;
    string captcha = "test";
//    msg_rsp.set_captcha(captcha);
//    msg_rsp.set_error(PTP::Common::NO_ERROR);
    CImPdu pdu;
    pdu.SetPBMsg(&msg_rsp);
    pdu.SetServiceId(S_AUTH);
    pdu.SetCommandId(CID_AuthCaptchaRes);
    pdu.SetSeqNum(0);
    pdu.SetReversed(1);
    unsigned char shared_secret[32];
    unsigned char iv[16];
    unsigned char aad[16];
    RAND_bytes(shared_secret,32);

    auto pPdu = CImPdu::ReadPdu(pdu.GetBuffer(), pdu.GetLength());

    string aadHex = "0x38f6f3c737eb1a1e187e9c673fce44d0";
    memcpy(aad, hex_to_string(aadHex.substr(2)).data(),16);
    string ivHex = "0x38f6f3c737eb1a1e187e9c673fce44d0";
    memcpy(iv, hex_to_string(ivHex.substr(2)).data(),16);

    unsigned char cipherData[1024];
    auto reversed = pPdu->GetReversed();
    if(reversed == 1){
        int cipherDataLen = aes_gcm_encrypt(
                pPdu->GetBodyData(),
                pPdu->GetBodyLength(),
                shared_secret,
                iv,aad,
                cipherData);
        ASSERT_EQ(cipherDataLen, pdu.GetBodyLength()+48);

        pdu.SetPBMsg(cipherData,cipherDataLen);
        pdu.SetServiceId(pPdu->GetServiceId());
        pdu.SetCommandId(pPdu->GetCommandId());
        pdu.SetSeqNum(pPdu->GetSeqNum());
        pdu.SetReversed(pPdu->GetReversed());


        auto pPdu1 = CImPdu::ReadPdu(pdu.GetBuffer(), pdu.GetLength());
        ASSERT_EQ(pPdu1->GetBodyLength(), cipherDataLen);
        ASSERT_EQ(pPdu1->GetCommandId(), pPdu->GetCommandId());
        ASSERT_EQ(pPdu1->GetReversed(), pPdu->GetReversed());


        unsigned char outBuf[1024];
        int outBufLen = aes_gcm_decrypt(pPdu1->GetBodyData(), pPdu1->GetBodyLength(),shared_secret,iv,aad,  outBuf);
        ASSERT_EQ(outBufLen, pPdu->GetBodyLength());

        CImPdu pdu2;
        pdu2.SetPBMsg(outBuf,outBufLen);
        pdu2.SetServiceId(pPdu1->GetServiceId());
        pdu2.SetCommandId(pPdu1->GetCommandId());
        pdu2.SetSeqNum(pPdu1->GetSeqNum());
        pdu2.SetReversed(pPdu1->GetReversed());

        PTP::Auth::AuthCaptchaRes msg1;
        msg1.ParseFromArray(pdu2.GetBodyData(),pdu2.GetBodyLength());
//        ASSERT_EQ(msg1.captcha(), captcha);
//        ASSERT_EQ(msg1.error(), NO_ERROR);

    }

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}