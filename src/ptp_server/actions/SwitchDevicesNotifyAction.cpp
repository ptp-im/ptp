/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  SwitchDevicesNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc：
 *
================================================================*/
#include "SwitchDevicesNotifyAction.h"
#include "PTP.Switch.pb.h"

using namespace PTP::Common;

namespace ACTION_SWITCH {
    void SwitchDevicesNotifyAction(CRequest* request, CResponse *response){
        PTP::Switch::SwitchDevicesNotify msg; 
        while (true){
            if(!msg.ParseFromArray(request->GetPdu()->GetBodyData(), (int)request->GetPdu()->GetBodyLength()))
            {
                break;
            }
            break;
        }
    }
};

