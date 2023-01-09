/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  AuthLogin.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Saturday, November 5, 2022
 *   desc：
 *
================================================================*/

#include "util.h"
#include "ImPduBase.h"
#include "ImUser.h"
#include "AttachData.h"
#include "PTP.Server.pb.h"
#include "PTP.Auth.pb.h"

using namespace PTP::Common;

namespace COMMAND {
    void AuthLoginReqCmd(CImPdu* pPdu, uint32_t conn_uuid){
        PTP::Auth::AuthLoginReq msg;
        PTP::Auth::AuthLoginRes msg_rsp;
//        log("conn_uuid=%u", conn_uuid);
        ERR error = NO_ERROR;
        auto pMsgConn = FindWebSocketConnByHandle(conn_uuid);

        if(!pMsgConn){
            log_error("not found pMsgConn");
            return;
        }
        while (true){
            if (pMsgConn->GetAddressHex().length() != 0) {
                log("duplicate LoginRequest in the same conn ");
                error = E_LOGIN_ERROR;
                break;
            }

            if (pMsgConn->GetCaptcha().length() == 0) {
                log("captcha is null ");
                error = E_LOGIN_ERROR;
                break;
            }

            if(!msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
            {
                error = E_PB_PARSE_ERROR;
                break;
            }
            string captcha = msg.captcha();

            if (captcha != pMsgConn->GetCaptcha()) {
                log("invalid captcha!");
                error = E_LOGIN_ERROR;
                break;
            }
            pMsgConn->SetCaptcha("");

            CDBServConn *pDbConn = get_db_serv_conn_for_login();
            if (!pDbConn)
            {
                error = E_REASON_NO_DB_SERVER;
                break;
            }
            else if (!pMsgConn->is_login_server_ok()) {
                error = E_REASON_NO_LOGIN_SERVER;
                break;
            }
            //else if (!is_route_server_available()) {
            //    error = E_REASON_NO_ROUTE_SERVER;
            //    break;
            //}

            const string& address = msg.address();
            pMsgConn->SetAddressHex(address);
            auto online_status = USER_STAT_ONLINE;
            pMsgConn->SetProperty(msg.client_version(), msg.client_type(), online_status);
            log_debug("address=%s", address.c_str());
            log_debug("client_type=%u ", msg.client_type());
            log_debug("client_version=%s ",  msg.client_version().c_str());
            pMsgConn->SetLoginTime(time(nullptr));

            CImUser *pImUser = CImUserManager::GetInstance()->GetImUserByAddressHex(pMsgConn->GetAddressHex());
            if (!pImUser) {
                pImUser = new CImUser(pMsgConn->GetAddressHex());
                CImUserManager::GetInstance()->AddImUserByAddressHex(pMsgConn->GetAddressHex(), pImUser);
            }
            pImUser->AddUnValidateMsgConn(pMsgConn);

            CDbAttachData attach(ATTACH_TYPE_HANDLE, conn_uuid, 0);

            PTP::Server::ServerLoginReq msg1;
            msg1.set_address(msg.address());
            msg1.set_captcha(msg.captcha());
            msg1.set_sign(msg.sign());
            msg1.set_attach_data(attach.GetBuffer(), attach.GetLength());
            CImPdu pPdu1;
            pPdu1.SetPBMsg(&msg1);
            pPdu1.SetServiceId(S_AUTH);
            pPdu1.SetCommandId(CID_ServerLoginReq);
            pPdu1.SetSeqNum(pPdu->GetSeqNum());
            pDbConn->SendPdu(&pPdu1);
            break;
        }
   
        if(error!= NO_ERROR){
            msg_rsp.set_error(error);
            CImPdu pdu;
            pdu.SetPBMsg(&msg_rsp);
            pdu.SetServiceId(S_AUTH);
            pdu.SetCommandId(CID_AuthLoginRes);
            pdu.SetSeqNum(pPdu->GetSeqNum());
            pMsgConn->SendPdu(&pdu);
        }
    }

};

