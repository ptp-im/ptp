/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyStatNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "BuddyStatNotifyAction.h"
#include "PTP.Buddy.pb.h"
#include "models/ModelBuddy.h"

using namespace PTP::Common;

namespace ACTION_BUDDY {
    void BuddyStatNotifyAction(CRequest* request){
        PTP::Buddy::BuddyStatNotify msg; 
        while (true){
            if(!msg.ParseFromArray(request->GetRequestPdu()->GetBodyData(), (int)request->GetRequestPdu()->GetBodyLength()))
            {
                break;
            }
            break;
        }
    }
};

