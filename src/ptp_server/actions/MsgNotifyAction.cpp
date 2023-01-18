/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "MsgNotifyAction.h"
#include "PTP.Msg.pb.h"
#include "models/ModelMsg.h"

using namespace PTP::Common;

namespace ACTION_MSG {
    void MsgNotifyAction(CRequest* request){
        PTP::Msg::MsgNotify msg; 
        while (true){
            if(!msg.ParseFromArray(request->GetRequestPdu()->GetBodyData(), (int)request->GetRequestPdu()->GetBodyLength()))
            {
                break;
            }
            break;
        }
    }
};

