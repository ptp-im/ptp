/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  SwitchPtpNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "SwitchPtpNotifyAction.h"
#include "PTP.Switch.pb.h"
#include "models/ModelSwitch.h"

using namespace PTP::Common;

namespace ACTION_SWITCH {
    void SwitchPtpNotifyAction(CRequest* request){
        PTP::Switch::SwitchPtpNotify msg; 
        while (true){
            if(!msg.ParseFromArray(request->GetRequestPdu()->GetBodyData(), (int)request->GetRequestPdu()->GetBodyLength()))
            {
                break;
            }
            break;
        }
    }
};

