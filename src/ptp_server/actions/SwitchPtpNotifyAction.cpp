/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  SwitchPtpNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc：
 *
================================================================*/
#include "SwitchPtpNotifyAction.h"
#include "PTP.Switch.pb.h"

using namespace PTP::Common;

namespace ACTION_SWITCH {
    void SwitchPtpNotifyAction(CRequest* request, CResponse *response){
        PTP::Switch::SwitchPtpNotify msg; 
        while (true){
            if(!msg.ParseFromArray(request->GetPdu()->GetBodyData(), (int)request->GetPdu()->GetBodyLength()))
            {
                break;
            }
            break;
        }
    }
};

