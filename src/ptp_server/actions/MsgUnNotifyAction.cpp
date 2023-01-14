/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgUnNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc：
 *
================================================================*/
#include "MsgUnNotifyAction.h"
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

namespace ACTION_MSG {
    void MsgUnNotifyAction(CRequest* request, CResponse *response){
        PTP::Msg::MsgUnNotify msg; 
        while (true){
            if(!msg.ParseFromArray(request->GetPdu()->GetBodyData(), (int)request->GetPdu()->GetBodyLength()))
            {
                break;
            }
            break;
        }
    }
};

