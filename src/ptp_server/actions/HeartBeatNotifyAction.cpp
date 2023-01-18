/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  HeartBeatNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "HeartBeatNotifyAction.h"
#include "PTP.Other.pb.h"
#include "models/ModelOther.h"

using namespace PTP::Common;

namespace ACTION_OTHER {
    void HeartBeatNotifyAction(CRequest* request){
        PTP::Other::HeartBeatNotify msg; 
        while (true){
            if(!msg.ParseFromArray(request->GetRequestPdu()->GetBodyData(), (int)request->GetRequestPdu()->GetBodyLength()))
            {
                break;
            }
            break;
        }
    }
};

