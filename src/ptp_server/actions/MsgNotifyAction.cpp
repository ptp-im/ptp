/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc：
 *
================================================================*/
#include "MsgNotifyAction.h"
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

namespace ACTION_MSG {
    void MsgNotifyAction(CRequest* request, CResponse *response){
        PTP::Msg::MsgNotify msg; 
        while (true){
            if(!msg.ParseFromArray(request->GetPdu()->GetBodyData(), (int)request->GetPdu()->GetBodyLength()))
            {
                break;
            }
            break;
        }
    }
};

