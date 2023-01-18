/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  SwitchDevicesNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "SwitchDevicesNotifyAction.h"
#include "PTP.Switch.pb.h"
#include "models/ModelSwitch.h"

using namespace PTP::Common;

namespace ACTION_SWITCH {
    void SwitchDevicesNotifyAction(CRequest* request){
        PTP::Switch::SwitchDevicesNotify msg; 
        while (true){
            if(!msg.ParseFromArray(request->GetRequestPdu()->GetBodyData(), (int)request->GetRequestPdu()->GetBodyLength()))
            {
                break;
            }
            break;
        }
    }
};

