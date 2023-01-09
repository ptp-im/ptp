/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgNotify.cpp
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
    void MsgNotifyCmd(CImPdu* pPdu){
        log_debug("MsgNotify start...");
        PTP::Msg::MsgNotify msg;
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
            pdu_rsp.SetCommandId(PTP::Common::CID_MsgNotify);
            pdu_rsp.SetSeqNum(0);

            PTP::Msg::MsgUnNotify msg_un_notify;
            if (msg.notify_users().size() > 0) {
                for (auto it = msg.notify_users().begin(); it != msg.notify_users().end(); it++) {
                    uint32_t toUserId = *it;
                    CImUser *pToImUser = CImUserManager::GetInstance()->GetImUserById(toUserId);
                    if(toUserId != msg.auth_uid()){
                        if (pToImUser) {
                            log_debug("notify from: %d -> toUserId: %d",msg.auth_uid(),toUserId);
                            pToImUser->BroadcastClientMsgData(&pdu_rsp, msg.msg_info().msg_id(), pMsgConn, msg.auth_uid());
                        }else{
                            msg_un_notify.add_un_notify_users(toUserId);
                            msg_un_notify.set_group_id(msg.msg_info().group_id());
                            msg_un_notify.set_sent_at(msg.msg_info().sent_at());
                            msg_un_notify.set_from_uid(msg.msg_info().from_uid());
                        }
                    }

                }
            }

            if(msg_un_notify.un_notify_users().size() > 0){
                log_debug("msg_un_notify size: %d", msg_un_notify.un_notify_users().size());
                CDBServConn* pDbConn = get_db_serv_conn();
                if(pDbConn){
                    CImPdu pdu;
                    pdu.SetPBMsg(&msg_un_notify);
                    pdu.SetServiceId(PTP::Common::S_MSG);
                    pdu.SetCommandId(PTP::Common::CID_MsgUnNotify);
                    pdu.SetSeqNum(0);
                    pDbConn->SendPdu(&pdu);
                }else{
                    log_error("msg_un_notify error: no pDbConn");
                }
            }
            break;
        }
        log_debug("MsgNotify end...");
    }
};

