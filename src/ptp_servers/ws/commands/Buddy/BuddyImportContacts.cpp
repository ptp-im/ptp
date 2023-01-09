/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyImportContacts.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Saturday, December 17, 2022
 *   desc：
 *
================================================================*/

#include "ptp_global/Util.h"
#include "ptp_global/ImPduBase.h"
#include "ImUser.h"
#include "AttachData.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

namespace COMMAND {
    void BuddyImportContactsReqCmd(CImPdu* pPdu, uint32_t conn_uuid){
        DEBUG_D("BuddyImportContactsReq start...");
        PTP::Buddy::BuddyImportContactsReq msg; 
        PTP::Buddy::BuddyImportContactsRes msg_rsp;
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
            
            CDBServConn* pDbConn = get_db_serv_conn();
            if (!pDbConn) {
                error = E_REASON_NO_DB_SERVER;
                DEBUG_E("not found pDbConn");
                break;
            }
            CDbAttachData attach(ATTACH_TYPE_HANDLE, conn_uuid, 0);
            msg.set_auth_uid(pMsgConn->GetUserId());
            msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
            pPdu->SetPBMsg(&msg);
            pDbConn->SendPdu(pPdu);
            break;
        }
        if(error!= NO_ERROR){
            
            msg_rsp.set_error(error);
                            
            CImPdu pdu;
            pdu.SetPBMsg(&msg_rsp);
            pdu.SetServiceId(S_BUDDY);
            pdu.SetCommandId(CID_BuddyImportContactsRes);
            pdu.SetSeqNum(pPdu->GetSeqNum());
            pMsgConn->SendPdu(&pdu);
        }
        //CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
        DEBUG_D("BuddyImportContactsReq end...");
    }
    void BuddyImportContactsResCmd(CImPdu* pPdu){
        DEBUG_D("BuddyImportContactsRes start...");
        PTP::Buddy::BuddyImportContactsRes msg;
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
            pdu_rsp.SetCommandId(PTP::Common::CID_BuddyImportContactsRes);
            pdu_rsp.SetSeqNum(pPdu->GetSeqNum());
            pMsgConn->SendPdu(&pdu_rsp);
            break;
        }
        DEBUG_D("BuddyImportContactsRes end...");
    }
};

