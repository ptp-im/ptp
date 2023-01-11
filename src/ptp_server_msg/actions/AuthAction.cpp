#include "AuthAction.h"
#include "PTP.Server.pb.h"
#include "PTP.Auth.pb.h"
#include "services/InterLogin.h"
#include "ptp_global/AttachData.h"
#include "ptp_crypto/secp256k1_helpers.h"
#include "ptp_crypto/crypto_helpers.h"
#include "MsgSrvConn.h"
#include "ImUser.h"

using namespace PTP::Common;

CInterLoginStrategy g_loginStrategy;

namespace ACTION {
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

            string sign = msg.sign();
            string captcha = msg.captcha();
            string address = msg.address();
            PTP::Common::UserInfo* pUser = msg_rsp.mutable_user_info();
            g_loginStrategy.ServerLogin(sign, captcha, address, pUser, error);
            DEBUG_D("login Ok uid=%d",pUser->uid());
        } while(false);

        msg_rsp.set_attach_data(msg.attach_data());
        msg_rsp.set_error(error);
        response->Next(&msg_rsp,CID_ServerLoginRes,request->GetPdu()->GetSeqNum());
    }

    void ServerLoginRes(CRequest* request, CResponse* response){
        PTP::Server::ServerLoginRes msg;
        PTP::Auth::AuthLoginRes msg_rsp;
        PTP::Common::ERR error = PTP::Common::NO_ERROR;
        CHECK_PB_PARSE_MSG(msg.ParseFromArray(request->GetPdu()->GetBodyData(), request->GetPdu()->GetBodyLength()));
        while (true){
            CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
            uint32_t handle = attach_data.GetHandle();
            msg.clear_attach_data();
            DEBUG_D("handle=%u", handle);
            auto pMsgConn = FindMsgSrvConnByHandle(handle);
            if(!pMsgConn){
                DEBUG_E("not found pMsgConn");
                return;
            }

            uint32_t user_id = msg.user_info().uid();
            string address = msg.user_info().address();
            CImUser* pImUser = CImUserManager::GetInstance()->GetImUserByAddress(address);
            if (!pImUser) {
                DEBUG_E("ImUser for address=%s not exist", address.c_str());
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
            pMsgConn->SetUserId(user_id);
            DEBUG_D("server prvKey: %s", prvKey.c_str());
            DEBUG_D("client pubKey: %s", bytes_to_hex_string(pub_key_33,33).c_str());
            DEBUG_D("shared_secret: %s", bytes_to_hex_string(shared_secret,32).c_str());

            pImUser->SetUserId(user_id);
            pImUser->SetHandle(handle);
            CImUserManager::GetInstance()->AddImUserById(user_id, pImUser);
            pMsgConn->SetOpen();
            CImPdu pdu_rsp;
            msg_rsp.set_error(msg.error());
            response->SendMsg(&msg_rsp,CID_AuthLoginRes,request->GetPdu()->GetSeqNum());
            break;
        }

    }

}
