/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ServerLoginAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc：
 *
================================================================*/
#include "ServerLoginAction.h"
#include "PTP.Server.pb.h"
#include "PTP.Auth.pb.h"
#include "MsgSrvConn.h"
#include "ImUser.h"
#include "services/InterLogin.h"
#include "ptp_crypto/secp256k1_helpers.h"

using namespace PTP::Common;

CInterLoginStrategy g_loginStrategy;

namespace ACTION_SERVER {
    void ServerLoginReqAction(CRequest* request, CResponse *response){
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
    void ServerLoginResAction(CRequest* request, CResponse *response){
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
};

