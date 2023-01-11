#include "AuthAction.h"
#include "PTP.Server.pb.h"
#include "PTP.Auth.pb.h"
#include "services/InterLogin.h"
#include "ptp_global/AttachData.h"
#include "ptp_crypto/secp256k1_helpers.h"
#include "ptp_crypto/crypto_helpers.h"
#include "ptp_wallet/HDKey.h"
#include "MsgSrvConn.h"
#include "ImUser.h"

using namespace PTP::Common;

CInterLoginStrategy g_loginStrategy;

namespace ACTION_AUTH {

    void AuthCaptchaReq(CRequest* request, CResponse* response){
        PTP::Auth::AuthCaptchaReq msg;
        PTP::Auth::AuthCaptchaRes msg_rsp;
        ERR error = NO_ERROR;
        while (true){
            DEBUG_D("handle=%u", request->GetHandle());
            auto pMsgConn = FindMsgSrvConnByHandle(request->GetHandle());
            if(!pMsgConn){
                DEBUG_E("not found pMsgConn");
                return;
            }
            if(!msg.ParseFromArray(request->GetPdu()->GetBodyData(), (int)request->GetPdu()->GetBodyLength()))
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
            string prvKeyStr = hdKey.privateKey.to_hex();
            secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
            ecdsa_sign_recoverable(ctx,msg_data,hdKey.privateKey.data(),signOut65);
            msg_rsp.set_sign(signOut65,65);
            msg_rsp.set_address(hex_to_string(address.substr(2)));
            msg_rsp.set_iv(iv,16);
            msg_rsp.set_aad(aad,16);
            pMsgConn->SetPrvKey(prvKeyStr);
            pMsgConn->SetIv(bytes_to_hex_string(iv,16));
            pMsgConn->SetAad(bytes_to_hex_string(aad,16));
            DEBUG_D("server prvKey: %s",hdKey.privateKey.to_hex().c_str());
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
        response->SendMsg(&msg_rsp,
                             CID_AuthCaptchaRes,
                             request->GetPdu()->GetSeqNum());
    }

    void AuthLoginReq(CRequest* request, CResponse* response){
        PTP::Auth::AuthLoginReq msg;
        PTP::Auth::AuthLoginRes msg_rsp;
        ERR error = NO_ERROR;
        while (true){
            DEBUG_D("handle=%u", request->GetHandle());
            auto pMsgConn = FindMsgSrvConnByHandle(request->GetHandle());
            if(!pMsgConn){
                DEBUG_E("not found pMsgConn");
                return;
            }
            if (pMsgConn->GetAddress().length() != 0) {
                DEBUG_I("duplicate LoginRequest in the same conn ");
                error = E_LOGIN_ERROR;
                break;
            }

            if (pMsgConn->GetCaptcha().length() == 0) {
                DEBUG_I("captcha is null ");
                error = E_LOGIN_ERROR;
                break;
            }

            if(!msg.ParseFromArray(request->GetPdu()->GetBodyData(), (int)request->GetPdu()->GetBodyLength()))
            {
                error = E_PB_PARSE_ERROR;
                break;
            }
            string captcha = msg.captcha();

            if (captcha != pMsgConn->GetCaptcha()) {
                DEBUG_I("invalid captcha!");
                error = E_LOGIN_ERROR;
                break;
            }
            pMsgConn->SetCaptcha("");

            const string& address = msg.address();
            pMsgConn->SetAddress(address);

            pMsgConn->m_client_version = msg.client_version();
            pMsgConn->m_client_type = msg.client_type();
            pMsgConn->m_online_status = USER_STAT_ONLINE;

            DEBUG_D("address=%s", address.c_str());
            DEBUG_D("client_type=%u ", msg.client_type());
            DEBUG_D("client_version=%s ",  msg.client_version().c_str());
            pMsgConn->m_login_time = time(nullptr);

            CImUser *pImUser = CImUserManager::GetInstance()->GetImUserByAddress(pMsgConn->GetAddress());
            if (!pImUser) {
                pImUser = new CImUser(pMsgConn->GetAddress());
                CImUserManager::GetInstance()->AddImUserByAddress(pMsgConn->GetAddress(), pImUser);
            }
            pImUser->AddMsgConn(pMsgConn->GetHandle(),pMsgConn);

            CDbAttachData attach(ATTACH_TYPE_HANDLE, pMsgConn->GetHandle(), 0);

            PTP::Server::ServerLoginReq msg_next;
            msg_next.set_address(msg.address());
            msg_next.set_captcha(msg.captcha());
            msg_next.set_sign(msg.sign());
            msg_next.set_attach_data(attach.GetBuffer(), attach.GetLength());
            response->Next(&msg_next,CID_ServerLoginReq,request->GetPdu()->GetSeqNum());
            break;
        }

        if(error!= NO_ERROR){
            msg_rsp.set_error(error);
            response->SendMsg(&msg_rsp,CID_AuthLoginRes,request->GetPdu()->GetSeqNum());
        }
    }

    void ServerLoginReq(CRequest* request, CResponse* response){
        PTP::Server::ServerLoginReq msg;
        PTP::Server::ServerLoginRes msg_rsp;
        PTP::Common::ERR error = PTP::Common::NO_ERROR;
        do{
            if(!msg.ParseFromArray(request->GetPdu()->GetBodyData(), (int)request->GetPdu()->GetBodyLength()))
            {
                error = E_PB_PARSE_ERROR;
                break;
            }

            PTP::Common::UserInfo* pUser = msg_rsp.mutable_user_info();
            g_loginStrategy.ServerLogin(&msg, pUser, error);
            if(error == NO_ERROR){
                DEBUG_D("===>>> login Ok uid=%d",pUser->uid());
            }
        } while(false);

        msg_rsp.set_attach_data(msg.attach_data());
        msg_rsp.set_error(error);
        response->Next(&msg_rsp,CID_ServerLoginRes,request->GetPdu()->GetSeqNum());
    }

    void ServerLoginRes(CRequest* request, CResponse* response){
        PTP::Server::ServerLoginRes msg;
        PTP::Auth::AuthLoginRes msg_rsp;
        auto error = msg.error();

        while (true){
            if(error != PTP::Common::NO_ERROR){
                break;
            }

            auto ret1 = msg.ParseFromArray(request->GetPdu()->GetBodyData(), request->GetPdu()->GetBodyLength());
            if(!ret1){
                error = PTP::Common::E_PB_PARSE_ERROR;
                DEBUG_E("ParseFromArray error");
                break;
            }
            CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
            uint32_t handle = request->GetHandle();
            msg.clear_attach_data();

            auto pMsgConn = FindMsgSrvConnByHandle(handle);

            uint32_t user_id = msg.user_info().uid();
            string address = msg.user_info().address();
            CImUser* pImUser = CImUserManager::GetInstance()->GetImUserByAddress(address);
            if (!pImUser) {
                DEBUG_E("ImUser for address=%s not exist", address.c_str());
                error = PTP::Common::E_LOGIN_ERROR;
                break;
            }
            DEBUG_D("client address=%s", address.c_str());
            string pub_key = msg.user_info().pub_key();
            PTP::Common::UserInfo* user_info = msg_rsp.mutable_user_info();
            user_info->set_uid(user_id);
            user_info->set_address(address);
            user_info->set_user_name(msg.user_info().user_name());
            user_info->set_nick_name(msg.user_info().nick_name());
            user_info->set_sign_info(msg.user_info().sign_info());
            user_info->set_avatar(msg.user_info().avatar());
            user_info->set_status(msg.user_info().status());
            user_info->set_pub_key(pub_key);

            secp256k1_pubkey pub_key_raw;

            auto prvKey = pMsgConn->GetPrvKey();
            if(prvKey.empty()){
                error = PTP::Common::E_LOGIN_ERROR;
                DEBUG_E("prvKey is null");
                break;
            }
            unsigned char shared_secret[32];
            unsigned char pub_key_33[33];
            memcpy(pub_key_33,pub_key.data(),33);

            secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
            auto ret = secp256k1_ec_pubkey_parse(ctx,&pub_key_raw,pub_key_33,33);
            secp256k1_context_destroy(ctx);
            if(!ret){
                error = PTP::Common::E_LOGIN_ERROR;
                DEBUG_E("parse pub_key_raw error");
                break;
            }
            ret = ecdh_create_share_key(&pub_key_raw,prvKey,shared_secret);
            if(!ret){
                error = PTP::Common::E_LOGIN_ERROR;
                DEBUG_E("gen share key error");
                break;
            }
            pMsgConn->SetShareKey(bytes_to_hex_string(shared_secret,32));
            pMsgConn->SetUserId(user_id);
            DEBUG_D("server prvKey: %s", prvKey.c_str());
            DEBUG_D("client pubKey: %s", bytes_to_hex_string(pub_key_33,33).c_str());
            DEBUG_D("shared_secret: %s", bytes_to_hex_string(shared_secret,32).c_str());

            pImUser->SetUserId(user_id);
            pImUser->SetHandle(handle);
            CImUserManager::GetInstance()->AddImUserById(user_id, pImUser);
            pMsgConn->SetOpen();
            DEBUG_D(">> pMsgConn << open");
            break;
        }
        msg_rsp.set_error(error);
        response->SendMsg(&msg_rsp,CID_AuthLoginRes,request->GetPdu()->GetSeqNum());
    }
}
