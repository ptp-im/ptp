/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  AuthLoginAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc：
 *
================================================================*/
#include "AuthLoginAction.h"
#include "PTP.Auth.pb.h"
#include "MsgSrvConn.h"
#include "PTP.Server.pb.h"
#include "ptp_crypto/crypto_helpers.h"
#include "ImUser.h"

using namespace PTP::Common;

namespace ACTION_AUTH {
    void AuthLoginReqAction(CRequest* request){
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

            if(!msg.ParseFromArray(request->GetRequestPdu()->GetBodyData(), (int)request->GetRequestPdu()->GetBodyLength()))
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

            string address = msg.address();
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

            PTP::Server::ServerLoginReq msg_next;
            msg_next.set_address(address);
            msg_next.set_captcha(msg.captcha());
            msg_next.set_sign(msg.sign());
            request->Next(&msg_next,CID_ServerLoginReq,request->GetSeqNum());
            break;
        }

        if(error!= NO_ERROR){
            msg_rsp.set_error(error);
            request->SendResponseMsg(&msg_rsp,CID_AuthLoginRes,request->GetSeqNum());
        }
    }
    
};

