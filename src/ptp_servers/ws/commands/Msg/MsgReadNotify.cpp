/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgReadNotify.cpp
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
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

namespace COMMAND {
    void MsgReadNotifyCmd(CImPdu* pPdu){
        log_debug("MsgReadNotify start...");
        PTP::Msg::MsgReadNotify msg;
        CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
        while (true){
            CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
            uint32_t handle = attach_data.GetHandle();
            log("userId=%u,handle=%d", msg.auth_uid(),handle);
            CMsgConn* pMsgConn = CImUserManager::GetInstance()->GetMsgConnByHandle(msg.auth_uid(), handle);
            msg.clear_attach_data();

            CImPdu pdu_rsp;
            pdu_rsp.SetPBMsg(&msg);
            pdu_rsp.SetServiceId(PTP::Common::S_MSG);
            pdu_rsp.SetCommandId(PTP::Common::CID_MsgReadNotify);
            pdu_rsp.SetSeqNum(0);
            PTP::Msg::MsgUnNotify msg_un_notify;
            if (msg.notify_users().size() > 0) {
                for (auto it = msg.notify_users().begin(); it != msg.notify_users().end(); it++) {
                    uint32_t toUserId = *it;
                    CImUser *pToImUser = CImUserManager::GetInstance()->GetImUserById(toUserId);
                    if(toUserId != msg.auth_uid()){
                        if (pToImUser) {
                            log_debug("read notify notify from: %d -> toUserId: %d",msg.auth_uid(),toUserId);
                            pToImUser->BroadcastPdu(&pdu_rsp ,pMsgConn);
                        }
                    }
                }
            }

            break;
        }
        log_debug("MsgReadNotify end...");
    }
};

