/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgUnNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "MsgUnNotifyAction.h"
#include "PTP.Msg.pb.h"
#include "models/ModelMsg.h"

using namespace PTP::Common;

namespace ACTION_MSG {
    void MsgUnNotifyAction(CRequest* request){
        PTP::Msg::MsgUnNotify msg; 
        while (true){
            if(!msg.ParseFromArray(request->GetRequestPdu()->GetBodyData(), (int)request->GetRequestPdu()->GetBodyLength()))
            {
                break;
            }
            break;
        }
    }
};

