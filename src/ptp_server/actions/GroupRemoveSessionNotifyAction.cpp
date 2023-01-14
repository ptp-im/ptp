/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupRemoveSessionNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc：
 *
================================================================*/
#include "GroupRemoveSessionNotifyAction.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

namespace ACTION_GROUP {
    void GroupRemoveSessionNotifyAction(CRequest* request, CResponse *response){
        PTP::Group::GroupRemoveSessionNotify msg; 
        while (true){
            if(!msg.ParseFromArray(request->GetPdu()->GetBodyData(), (int)request->GetPdu()->GetBodyLength()))
            {
                break;
            }
            break;
        }
    }
};

