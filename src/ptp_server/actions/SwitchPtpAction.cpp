/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  SwitchPtpAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "SwitchPtpAction.h"
#include "PTP.Switch.pb.h"
#include "MsgSrvConn.h"
#include "ImUser.h"
#include "models/ModelSwitch.h"

using namespace PTP::Common;

namespace ACTION_SWITCH {
    void SwitchPtpReqAction(CRequest* request){
        PTP::Switch::SwitchPtpReq msg;
        PTP::Switch::SwitchPtpRes msg_rsp;
        PTP::Switch::SwitchPtpNotify msg_notify;

        ERR error = NO_ERROR;
        while (true){
            if(!msg.ParseFromArray(request->GetRequestPdu()->GetBodyData(), (int)request->GetRequestPdu()->GetBodyLength()))
            {
                error = E_PB_PARSE_ERROR;
                break;
            }
            if(!request->IsBusinessConn()){
              auto pMsgConn = FindMsgSrvConnByHandle(request->GetHandle());
              if(!pMsgConn){
                  DEBUG_E("not found pMsgConn");
                  return;
              }
                const string& to_adr = msg.to_adr();
                auto toUser = CImUserManager::GetInstance()->GetImUserByAddress(to_adr);
                msg_notify.set_from_adr(pMsgConn->GetAddress());
                msg_notify.set_data(msg.data());
                msg_notify.set_switch_type(msg.switch_type());
                ImPdu pdu;
                pdu.SetPBMsg(&msg_notify,CID_SwitchPtpNotify,0);
                pdu.SetReversed(1);
                if(toUser){
                    toUser->BroadcastPdu(&pdu);
                }else{
                    error = PTP::Common::E_SWITCH_USER_NO_ONLINE;
                }
            }
            break;
        }
        msg_rsp.set_error(error);
        request->SendResponseMsg(&msg_rsp,CID_SwitchPtpRes,request->GetSeqNum());
    }
};

