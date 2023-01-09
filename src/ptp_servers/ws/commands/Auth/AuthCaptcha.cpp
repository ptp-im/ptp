/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  AuthCaptcha.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Saturday, November 5, 2022
 *   desc：
 *
================================================================*/

#include "ptp_global/Util.h"
#include "ptp_global/ImPduBase.h"
#include "ImUser.h"
#include "PTP.Auth.pb.h"
#include "helpers.h"
#include "secp256k1_helpers.h"
#include <array>

using namespace PTP::Common;

namespace COMMAND {
    void AuthCaptchaReqCmd(CImPdu* pPdu, uint32_t conn_uuid){
        PTP::Auth::AuthCaptchaReq msg;
        PTP::Auth::AuthCaptchaRes msg_rsp;
        ERR error = NO_ERROR;

        auto pMsgConn = FindWebSocketConnByHandle(conn_uuid);
        if(!pMsgConn){
            DEBUG_E("not found pMsgConn");
            return;
        }

        while (true){
            if(!msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
            {
                error = E_PB_PARSE_ERROR;
                break;
            }
            auto captcha = gen_random(6);
            pMsgConn->SetCaptcha(captcha);
            msg_rsp.set_captcha(captcha);
            unsigned char iv[16] = { 0 };
            gen_random_bytes(iv,16);
            unsigned char aad[16] = { 0 };
            gen_random_bytes(aad,16);
            string msg_data = format_sign_msg_data(captcha,SignMsgType_ptp);
            unsigned char signOut65[65];
            string prvKeyStr = "0x1ffe0fafed803ef0f357c8678d00089404545e8a9a9f72fb41e559ddaa9c531c";
            string pubKey33Str = "0x02f0f6796be474b77707a0ae4962c20af84bb7fc3995c91fa2ee41d2803e101069";
            string pubKeyStr = "0xf0f6796be474b77707a0ae4962c20af84bb7fc3995c91fa2ee41d2803e1010698217cd09b4a0753f00df3c13cd12ea39cd93c3ffc6733886347207aafbc5ac40";
            auto pub_key = hex_to_string(pubKeyStr.substr(2));
            string address = "0xf2472d9e07c721da4bf74ddd5c587ca7f5b3ee60";
            secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
            auto prv_key = hex_to_string(prvKeyStr.substr(2));
            auto* prv_key_bytes = (unsigned char*) prv_key.data();
            ecdsa_sign_recoverable(ctx,msg_data,prv_key_bytes,signOut65);
            secp256k1_context_destroy(ctx);
            msg_rsp.set_sign(signOut65,65);
            msg_rsp.set_address(hex_to_string(address.substr(2)));
            msg_rsp.set_iv(iv,16);
            msg_rsp.set_aad(aad,16);
            pMsgConn->SetPrvKey(prvKeyStr);
            pMsgConn->SetIv(bytes_to_hex_string(iv,16));
            pMsgConn->SetAad(bytes_to_hex_string(aad,16));
            //DEBUG_D("server pubKey: %s",pubKey33Str.c_str());
            //DEBUG_D("msg_data: %s",msg_data.c_str());
            //DEBUG_D("signOut65: %s", bytes_to_hex_string(signOut65,65).c_str());
            //DEBUG_D("captcha: %s",msg_rsp.captcha().c_str());
            //DEBUG_D("iv: %s", bytes_to_hex_string(iv,16).c_str());
            //DEBUG_D("aad: %s", bytes_to_hex_string(aad,16).c_str());
            break;
        }
        msg_rsp.set_error(error);
        CImPdu pdu;
        pdu.SetPBMsg(&msg_rsp);
        pdu.SetServiceId(S_AUTH);
        pdu.SetCommandId(CID_AuthCaptchaRes);
        pdu.SetSeqNum(pPdu->GetSeqNum());
        pMsgConn->SendPdu(&pdu);
    }
};

