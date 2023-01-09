/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupModifyNotify.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Saturday, November 5, 2022
 *   desc：
 *
================================================================*/

#include "ptp_global/Util.h"
#include "ptp_global/ImPduBase.h"
#include "ImUser.h"
#include "AttachData.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

namespace COMMAND {
    void GroupModifyNotifyCmd(CImPdu* pPdu){
        DEBUG_D("GroupModifyNotify start...");
        PTP::Group::GroupModifyNotify msg;
        CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
        while (true){
            // CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
            // uint32_t conn_uuid = attach_data.GetHandle();
            // msg.clear_attach_data();
            // DEBUG_I("conn_uuid=%u", conn_uuid);
            // auto pMsgConn = FindWebSocketConnByHandle(conn_uuid);
            // if(!pMsgConn){
            //     DEBUG_E("not found pMsgConn");
            //     return;
            // }
            // CImPdu pdu_rsp;
            // pdu_rsp.SetPBMsg(&msg);
            // pdu_rsp.SetServiceId(PTP::Common::S_GROUP);
            // pdu_rsp.SetCommandId(PTP::Common::CID_GroupModifyNotify);
            // pdu_rsp.SetSeqNum(pPdu->GetSeqNum());
            // pMsgConn->SendPdu(&pdu_rsp);
            break;
        }
        DEBUG_D("GroupModifyNotify end...");
    }
};

