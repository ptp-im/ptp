/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupPreCreate.cpp
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
#include "ptp_global/Helpers.h"

using namespace PTP::Common;

namespace COMMAND {
    void GroupPreCreateReqCmd(CImPdu* pPdu, uint32_t conn_uuid){
        DEBUG_D("GroupPreCreateReq start...");
        PTP::Group::GroupPreCreateReq msg; 
        PTP::Group::GroupPreCreateRes msg_rsp;
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

            if (msg.group_type() == GROUP_TYPE_PAIR && msg.members().size() != 1) {
                error = E_GROUP_CREATE_PAIR_GROUP_MEMBER_SIE_INVALID;
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
            pdu.SetCommandId(CID_GroupPreCreateRes);
            pdu.SetSeqNum(pPdu->GetSeqNum());
            pMsgConn->SendPdu(&pdu);
            //CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
        }
        DEBUG_D("GroupPreCreateReq end...");
    }
    
    void GroupPreCreateResCmd(CImPdu* pPdu){
        DEBUG_D("GroupPreCreateRes start...");
        PTP::Group::GroupPreCreateRes msg;
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
//            auto captcha = gen_random(6);
//            pMsgConn->SetCaptcha(captcha);
//            msg.set_captcha(captcha);
//            ImPdu pdu_rsp;
//            pdu_rsp.SetPBMsg(&msg);
//            pdu_rsp.SetServiceId(PTP::Common::S_GROUP);
//            pdu_rsp.SetCommandId(PTP::Common::CID_GroupPreCreateRes);
//            pdu_rsp.SetSeqNum(pPdu->GetSeqNum());
//            pMsgConn->SendPdu(&pdu_rsp);
            break;
        }
        DEBUG_D("GroupPreCreateRes end...");
    }
};

