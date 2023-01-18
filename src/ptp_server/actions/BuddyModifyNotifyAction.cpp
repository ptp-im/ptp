/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyModifyNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "BuddyModifyNotifyAction.h"
#include "PTP.Buddy.pb.h"
#include "models/ModelBuddy.h"

using namespace PTP::Common;

namespace ACTION_BUDDY {
    void BuddyModifyNotifyAction(CRequest* request){
        PTP::Buddy::BuddyModifyNotify msg; 
        while (true){
            if(!msg.ParseFromArray(request->GetRequestPdu()->GetBodyData(), (int)request->GetRequestPdu()->GetBodyLength()))
            {
                break;
            }
            break;
        }
    }
};

