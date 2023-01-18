/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupModify.cpp
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
    void GroupModifyReqCmd(CImPdu* pPdu, uint32_t conn_uuid){
        DEBUG_D("GroupModifyReq start...");
        PTP::Group::GroupModifyReq msg; 
        PTP::Group::GroupModifyRes msg_rsp;
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
            CDbAttachData attach(ATTACH_TYPE_HANDLE, conn_uuid, 0);
            msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
            msg.set_auth_uid(pMsgConn->GetUserId());
            pPdu->SetPBMsg(&msg);
            pDbConn->SendPdu(pPdu);

            break;
        }
   
        if(error!= NO_ERROR){
            
            msg_rsp.set_error(error);
                            
            ImPdu pdu;
            pdu.SetPBMsg(&msg_rsp);
            pdu.SetServiceId(S_GROUP);
            pdu.SetCommandId(CID_GroupModifyRes);
            pdu.SetSeqNum(pPdu->GetSeqNum());
            pMsgConn->SendPdu(&pdu);
            //CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
        }
        DEBUG_D("GroupModifyReq end...");
    }
    
    void GroupModifyResCmd(CImPdu* pPdu){
        DEBUG_D("GroupModifyRes start...");
        PTP::Group::GroupModifyRes msg;
        CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
        while (true){
            CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
            uint32_t conn_uuid = attach_data.GetHandle();
            msg.clear_attach_data();
            auto notify_members = msg.notify_members();
            msg.clear_notify_members();
            DEBUG_I("conn_uuid=%u", conn_uuid);
            auto pMsgConn = FindWebSocketConnByHandle(conn_uuid);
            if(!pMsgConn){
                DEBUG_E("not found pMsgConn");
                return;
            }

            ImPdu pdu_rsp;
            pdu_rsp.SetPBMsg(&msg);
            pdu_rsp.SetServiceId(PTP::Common::S_GROUP);
            pdu_rsp.SetCommandId(PTP::Common::CID_GroupModifyRes);
            pdu_rsp.SetSeqNum(pPdu->GetSeqNum());
            pMsgConn->SendPdu(&pdu_rsp);

            PTP::Group::GroupModifyNotify msg_notify;
            msg_notify.set_value(msg.value());
            msg_notify.set_group_modify_action(msg.group_modify_action());
            ImPdu pdu2;

            pdu2.SetPBMsg(&msg_notify);
            pdu2.SetServiceId(PTP::Common::S_GROUP);
            pdu2.SetCommandId(PTP::Common::CID_GroupModifyNotify);
            pdu2.SetSeqNum(0);

            for (auto it = notify_members.begin(); it != notify_members.end(); it++) {
                uint32_t toUserId = *it;
                CImUser *pToImUser = CImUserManager::GetInstance()->GetImUserById(toUserId);
                if (pToImUser && toUserId != msg.auth_uid()) {
                    DEBUG_I("CID_GroupModifyNotify: %d -> %d",msg.auth_uid(),toUserId);
                    pToImUser->BroadcastPdu(&pdu2, pMsgConn);
                }
            }

            break;
        }
        DEBUG_D("GroupModifyRes end...");
    }
};

