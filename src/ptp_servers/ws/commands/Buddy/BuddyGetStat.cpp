/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyGetStat.cpp
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
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

namespace COMMAND {
    void BuddyGetStatReqCmd(CImPdu* pPdu, uint32_t conn_uuid){
        log_debug("BuddyGetStatReq start...");
        PTP::Buddy::BuddyGetStatReq msg; 
        PTP::Buddy::BuddyGetStatRes msg_rsp;
        log("conn_uuid=%u", conn_uuid);
        ERR error = NO_ERROR;

        auto pMsgConn = FindWebSocketConnByHandle(conn_uuid);
        if(!pMsgConn){
            log_error("not found pMsgConn");
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
                log_error("not found pDbConn");
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
            pdu.SetCommandId(CID_BuddyGetStatRes);
            pdu.SetSeqNum(pPdu->GetSeqNum());
            pMsgConn->SendPdu(&pdu);
            //CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
        }
        log_debug("BuddyGetStatReq end...");
    }
    
    void BuddyGetStatResCmd(CImPdu* pPdu){
        log_debug("BuddyGetStatRes start...");
        PTP::Buddy::BuddyGetStatRes msg;
        CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
        while (true){
            CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
            uint32_t conn_uuid = attach_data.GetHandle();
            msg.clear_attach_data();
            log("conn_uuid=%u", conn_uuid);
            auto pMsgConn = FindWebSocketConnByHandle(conn_uuid);
            if(!pMsgConn){
                log_error("not found pMsgConn");
                return;
            }
            CImPdu pdu_rsp;
            pdu_rsp.SetPBMsg(&msg);
            pdu_rsp.SetServiceId(PTP::Common::S_BUDDY);
            pdu_rsp.SetCommandId(PTP::Common::CID_BuddyGetStatRes);
            pdu_rsp.SetSeqNum(pPdu->GetSeqNum());
            pMsgConn->SendPdu(&pdu_rsp);
            break;
        }
        log_debug("BuddyGetStatRes end...");
    }
};

