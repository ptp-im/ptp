/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupCreate.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Saturday, November 5, 2022
 *   desc：
 *
================================================================*/

#include "ptp_global/Utils.h"
#include "ptp_protobuf/ImPdu.h"
#include "ImUser.h"
#include "ptp_global/AttachData.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

namespace COMMAND {
    void GroupCreateReqCmd(CImPdu* pPdu, uint32_t conn_uuid){
        DEBUG_D("GroupCreateReq start...");
        PTP::Group::GroupCreateReq msg; 
        PTP::Group::GroupCreateRes msg_rsp;
        DEBUG_I("conn_uuid=%u", conn_uuid);
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
            CBusinessServConn* pDbConn = get_business_serv_conn();
            if (!pDbConn) {
                error = E_REASON_NO_DB_SERVER;
                DEBUG_E("not found pDbConn");
                break;
            }

            if(pMsgConn->GetCaptcha().empty()){
                error = E_SYSTEM;
                DEBUG_E("captcha is null");
                break;
            }

            if(pMsgConn->GetCaptcha() != msg.captcha()){
                error = E_SYSTEM;
                DEBUG_E("captcha is invalid");
                break;
            }
            pMsgConn->SetCaptcha("");

            CDbAttachData attach(ATTACH_TYPE_HANDLE, conn_uuid, 0);
            msg.set_auth_uid(pMsgConn->GetUserId());
            msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
            pPdu->SetPBMsg(&msg);
            pDbConn->SendPdu(pPdu);
            break;
        }
   
        if(error!= NO_ERROR){
            
            msg_rsp.set_error(error);
                            
            ImPdu pdu;
            pdu.SetPBMsg(&msg_rsp);
            pdu.SetServiceId(S_GROUP);
            pdu.SetCommandId(CID_GroupCreateRes);
            pdu.SetSeqNum(pPdu->GetSeqNum());
            pMsgConn->SendPdu(&pdu);
            //CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
        }
        DEBUG_D("GroupCreateReq end...");
    }
    
    void GroupCreateResCmd(CImPdu* pPdu){
        DEBUG_D("GroupCreateRes start...");
        PTP::Group::GroupCreateRes msg;
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
            ImPdu pdu_rsp;
            pdu_rsp.SetPBMsg(&msg);
            pdu_rsp.SetServiceId(PTP::Common::S_GROUP);
            pdu_rsp.SetCommandId(PTP::Common::CID_GroupCreateRes);
            pdu_rsp.SetSeqNum(pPdu->GetSeqNum());
            pMsgConn->SendPdu(&pdu_rsp);
            break;
        }
        DEBUG_D("GroupCreateRes end...");
    }
};

