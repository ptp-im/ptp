/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ServerLogin.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Saturday, November 5, 2022
 *   desc：
 *
================================================================*/

#include "ptp_global/Util.h"
#include "ptp_global/Helpers.h"
#include "ptp_global/ImPduBase.h"
#include "ImUser.h"
#include "ptp_global/AttachData.h"
#include "PTP.Auth.pb.h"
#include "PTP.Server.pb.h"
#include "ptp_crypto/secp256k1_helpers.h"


using namespace PTP::Common;

namespace COMMAND {
    void ServerLoginReqCmd(CImPdu* pPdu, uint32_t conn_uuid){
//        DEBUG_D("ServerLoginReq start...");
        PTP::Server::ServerLoginReq msg;
        PTP::Server::ServerLoginRes msg_rsp;
//        DEBUG_I("conn_uuid=%u", conn_uuid);
        ERR error = NO_ERROR;
        while (true){
            if(!msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
            {
                error = E_PB_PARSE_ERROR;
                break;
            }
            CDBServConn* pDbConn = get_db_serv_conn();
            if (!pDbConn) {
                error = E_REASON_NO_DB_SERVER;
                DEBUG_E("not found pDbConn");
                break;
            }
            CDbAttachData attach(ATTACH_TYPE_HANDLE, conn_uuid, 0);
            msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
            pPdu->SetPBMsg(&msg);
            pDbConn->SendPdu(pPdu);
            break;
        }

        if(error!= NO_ERROR){
            auto pMsgConn = FindWebSocketConnByHandle(conn_uuid);
            if(!pMsgConn){
                DEBUG_E("not found pMsgConn");
                return;
            }

            msg_rsp.set_error(error);

            CImPdu pdu;
            pdu.SetPBMsg(&msg_rsp);
            pdu.SetServiceId(S_SERVER);
            pdu.SetCommandId(CID_ServerLoginRes);
            pdu.SetSeqNum(pPdu->GetSeqNum());
            pMsgConn->SendPdu(&pdu);
            //CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
        }
//        DEBUG_D("ServerLoginReq end...");
    }

    void ServerLoginResCmd(CImPdu* pPdu){
//        DEBUG_D("ServerLoginRes start...");
        PTP::Auth::AuthLoginRes msg_rsp;
        PTP::Server::ServerLoginRes msg;
        CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
        while (true){
            CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
            uint32_t conn_uuid = attach_data.GetHandle();
            msg.clear_attach_data();
            DEBUG_I("conn_uuid=%u", conn_uuid);
            auto pMsgConn = FindWebSocketConnByHandle(conn_uuid);
            if(!pMsgConn){
                DEBUG_E("not found pMsgConn");
                return;
            }

            CImUser* pImUser = CImUserManager::GetInstance()->GetImUserByAddressHex(msg.user_info().address());
            if (!pImUser) {
                DEBUG_E("ImUser for user_name=%s not exist", msg.user_info().address().c_str());
                break;
            }
            string pub_key = msg.user_info().pub_key();

            PTP::Common::UserInfo* user_info = msg_rsp.mutable_user_info();
            user_info->set_uid(msg.user_info().uid());
            user_info->set_address(msg.user_info().address());
            user_info->set_user_name(msg.user_info().user_name());
            user_info->set_nick_name(msg.user_info().nick_name());
            user_info->set_sign_info(msg.user_info().sign_info());
            user_info->set_avatar(msg.user_info().avatar());
            user_info->set_status(msg.user_info().status());
            user_info->set_pub_key(pub_key);

            uint32_t user_id = msg.user_info().uid();

            secp256k1_pubkey pub_key_raw;

            auto prvKey = pMsgConn->GetPrvKey();
            if(prvKey.empty()){
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
                DEBUG_E("parse pub_key_raw error");
                break;
            }
            ret = ecdh_create_share_key(&pub_key_raw,prvKey,shared_secret);
            if(!ret){
                DEBUG_E("gen share key error");
                break;
            }
            pMsgConn->SetShareKey(bytes_to_hex_string(shared_secret,32));
            //DEBUG_I("server prvKey: %s", prvKey.c_str());
            //DEBUG_I("client pubKey: %s", bytes_to_hex_string(pub_key_33,33).c_str());
            //DEBUG_I("shared_secret: %s", bytes_to_hex_string(shared_secret,32).c_str());
            CImUser* pUser = CImUserManager::GetInstance()->GetImUserById(user_id);
            if (pUser)
            {
                pUser->AddUnValidateMsgConn(pMsgConn);
                pImUser->DelUnValidateMsgConn(pMsgConn);
                if (pImUser->IsMsgConnEmpty())
                {
                    CImUserManager::GetInstance()->RemoveImUserByAddressHex(msg.user_info().address());
                    delete pImUser;
                }
            }
            else
            {
                pUser = pImUser;
            }

            pUser->SetUserId(user_id);
            pUser->SetNickName(msg.user_info().nick_name());
            pUser->SetValidated();
            pUser->ValidateMsgConn(pMsgConn->GetHandle(), pMsgConn);
            CImUserManager::GetInstance()->AddImUserById(user_id, pUser);
            pMsgConn->SetUserId(user_id);
            pMsgConn->SetOpen();
            pMsgConn->SendUserStatusUpdate(USER_STAT_ONLINE);
            CImPdu pdu_rsp;
            msg_rsp.set_error(msg.error());
            pdu_rsp.SetPBMsg(&msg_rsp);
            pdu_rsp.SetServiceId(PTP::Common::S_AUTH);
            pdu_rsp.SetCommandId(PTP::Common::CID_AuthLoginRes);
            pdu_rsp.SetSeqNum(pPdu->GetSeqNum());
            pdu_rsp.SetReversed(1);
            pMsgConn->SendPdu(&pdu_rsp);
            break;
        }
    }
};

