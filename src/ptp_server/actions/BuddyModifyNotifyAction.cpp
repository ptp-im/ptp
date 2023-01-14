/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyModifyNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc：
 *
================================================================*/
#include "BuddyModifyNotifyAction.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

namespace ACTION_BUDDY {
    void BuddyModifyNotifyAction(CRequest* request, CResponse *response){
        PTP::Buddy::BuddyModifyNotify msg; 
        while (true){
            if(!msg.ParseFromArray(request->GetPdu()->GetBodyData(), (int)request->GetPdu()->GetBodyLength()))
            {
                break;
            }
            break;
        }
    }
};

