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

#include "ptp_global/Utils.h"
#include "ImUser.h"
#include "PTP.Auth.pb.h"
#include "ptp_protobuf/ImPdu.h"
#include "ptp_global/Helpers.h"
#include "ptp_crypto/secp256k1_helpers.h"
#include "ptp_crypto/crypto_helpers.h"
#include <array>
#include "ptp_wallet/HDKey.h"

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
            if(!msg.ParseFromArray(pPdu->GetBodyData(), (int)pPdu->GetBodyLength()))
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

            PTPWallet::MnemonicHelper::MnemonicResult mnemonic = PTPWallet::MnemonicHelper::generate();
            PTPWallet::HDKey hdKey = PTPWallet::HDKeyEncoder::makeEthRootKey(mnemonic.raw.data());
            PTPWallet::HDKeyEncoder::makeEthExtendedKey(hdKey, PTP_HD_PATH);
            string address = PTPWallet::HDKeyEncoder::getEthAddress(hdKey);
            string prvKeyStr = "0x" + hdKey.privateKey.to_hex();
            secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
            ecdsa_sign_recoverable(ctx,msg_data,hdKey.privateKey.data(),signOut65);
            msg_rsp.set_sign(signOut65,65);
            msg_rsp.set_address(hex_to_string(address.substr(2)));
            msg_rsp.set_iv(iv,16);
            msg_rsp.set_aad(aad,16);
            pMsgConn->SetPrvKey(prvKeyStr);
            pMsgConn->SetIv(bytes_to_hex_string(iv,16));
            pMsgConn->SetAad(bytes_to_hex_string(aad,16));
            DEBUG_D("server pubKey: %s",hdKey.publicKey.to_hex().c_str());
            DEBUG_D("server address: %s",address.c_str());
            DEBUG_D("msg_data: %s",msg_data.c_str());
            DEBUG_D("signOut65: %s", bytes_to_hex_string(signOut65,65).c_str());
            DEBUG_D("captcha: %s",msg_rsp.captcha().c_str());
            DEBUG_D("iv: %s", bytes_to_hex_string(iv,16).c_str());
            DEBUG_D("aad: %s", bytes_to_hex_string(aad,16).c_str());
            break;
        }
        msg_rsp.set_error(error);
        ImPdu pdu;
        pdu.SetPBMsg(
                &msg_rsp,
                CID_AuthCaptchaRes,
                pPdu->GetSeqNum());
        pMsgConn->SendPdu(&pdu);
    }
};

