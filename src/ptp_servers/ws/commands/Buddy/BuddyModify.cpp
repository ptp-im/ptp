/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyModify.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Saturday, November 5, 2022
 *   desc：
 *
================================================================*/

#include "ptp_global/Util.h"
#include "ptp_global/ImPduBase.h"
#include "ImUser.h"
#include "ptp_global/AttachData.h"
#include "PTP.Buddy.pb.h"
#include "PTP.Server.pb.h"

using namespace PTP::Common;

namespace COMMAND {
    void BuddyModifyReqCmd(CImPdu* pPdu, uint32_t conn_uuid){
        DEBUG_D("BuddyModifyReq start...");
        PTP::Buddy::BuddyModifyReq msg; 
        PTP::Buddy::BuddyModifyRes msg_rsp;
        DEBUG_I("conn_uuid=%u", conn_uuid);

        auto pMsgConn = FindWebSocketConnByHandle(conn_uuid);
        if(!pMsgConn){
            DEBUG_E("not found pMsgConn");
            return;
        }

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
            msg.set_auth_uid(pMsgConn->GetUserId());
            pPdu->SetPBMsg(&msg);
            pDbConn->SendPdu(pPdu);
            break;
        }
   
        if(error!= NO_ERROR){
            msg_rsp.set_error(error);
                            
            CImPdu pdu;
            pdu.SetPBMsg(&msg_rsp);
            pdu.SetServiceId(S_BUDDY);
            pdu.SetCommandId(CID_BuddyModifyRes);
            pdu.SetSeqNum(pPdu->GetSeqNum());
            pMsgConn->SendPdu(&pdu);
            //CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
        }
        DEBUG_D("BuddyModifyReq end...");
    }
    
    void BuddyModifyResCmd(CImPdu* pPdu){
        DEBUG_D("BuddyModifyRes start...");
        PTP::Buddy::BuddyModifyRes msg;
        PTP::Buddy::BuddyModifyUpdatePair msg1;
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
            CImPdu pdu_rsp;
            pdu_rsp.SetPBMsg(&msg);
            pdu_rsp.SetServiceId(PTP::Common::S_BUDDY);
            pdu_rsp.SetCommandId(PTP::Common::CID_BuddyModifyRes);
            pdu_rsp.SetSeqNum(pPdu->GetSeqNum());
            pMsgConn->SendPdu(&pdu_rsp);

            PTP::Buddy::BuddyModifyNotify msgNotify;
            msgNotify.set_uid(msg.auth_uid());
            msgNotify.set_value(msg.value());
            msgNotify.set_buddy_modify_action(msg.buddy_modify_action());

            CImPdu pdu_notify;
            pdu_notify.SetPBMsg(&msgNotify);
            pdu_notify.SetServiceId(PTP::Common::S_BUDDY);
            pdu_notify.SetCommandId(PTP::Common::CID_BuddyModifyNotify);
            pdu_notify.SetSeqNum(0);

            auto notify_pairs = msg.notify_pairs();
            msg.clear_notify_pairs();
            list<uint32_t> user_ids;
            for (auto it = notify_pairs.begin(); it != notify_pairs.end(); it++) {
                uint32_t toUserId = *it;
                CImUser *pToImUser = CImUserManager::GetInstance()->GetImUserById(toUserId);
                if (pToImUser) {
                    DEBUG_I("CID_BuddyModifyNotify: %d -> %d",msg.auth_uid(),toUserId);
                    pToImUser->BroadcastPdu(&pdu_notify, pMsgConn);
                }else{
                    msg1.add_pair_uid_list(toUserId);
                }
            }

            msg1.set_auth_uid(pMsgConn->GetUserId());
            CImPdu pdu1;
            pdu1.SetPBMsg(&msg1);
            pdu1.SetServiceId(PTP::Common::S_BUDDY);
            pdu1.SetCommandId(PTP::Common::CID_BuddyModifyUpdatePair);
            pdu1.SetSeqNum(0);
            CDBServConn* pDbConn = get_db_serv_conn();
            if (!pDbConn) {
                DEBUG_E("not found pDbConn");
                break;
            }
            pDbConn->SendPdu(&pdu1);
            break;
        }
        DEBUG_D("BuddyModifyRes end...");
    }
};

