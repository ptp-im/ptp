/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  HeartBeatNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc：
 *
================================================================*/
#include "HeartBeatNotifyAction.h"
#include "PTP.Other.pb.h"

using namespace PTP::Common;

namespace ACTION_OTHER {
    void HeartBeatNotifyAction(CRequest* request, CResponse *response){
        PTP::Other::HeartBeatNotify msg; 
        while (true){
            if(!msg.ParseFromArray(request->GetPdu()->GetBodyData(), (int)request->GetPdu()->GetBodyLength()))
            {
                break;
            }
            break;
        }
    }
};

