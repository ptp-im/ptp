/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgReadNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "MsgReadNotifyAction.h"
#include "PTP.Msg.pb.h"
#include "models/ModelMsg.h"

using namespace PTP::Common;

namespace ACTION_MSG {
    void MsgReadNotifyAction(CRequest* request){
        PTP::Msg::MsgReadNotify msg; 
        while (true){
            if(!msg.ParseFromArray(request->GetRequestPdu()->GetBodyData(), (int)request->GetRequestPdu()->GetBodyLength()))
            {
                break;
            }
            break;
        }
    }
};

