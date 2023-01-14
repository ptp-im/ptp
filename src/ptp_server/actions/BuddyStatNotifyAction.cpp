/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyStatNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc：
 *
================================================================*/
#include "BuddyStatNotifyAction.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

namespace ACTION_BUDDY {
    void BuddyStatNotifyAction(CRequest* request, CResponse *response){
        PTP::Buddy::BuddyStatNotify msg; 
        while (true){
            if(!msg.ParseFromArray(request->GetPdu()->GetBodyData(), (int)request->GetPdu()->GetBodyLength()))
            {
                break;
            }
            break;
        }
    }
};

