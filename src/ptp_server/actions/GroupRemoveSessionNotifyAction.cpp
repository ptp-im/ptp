/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupRemoveSessionNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "GroupRemoveSessionNotifyAction.h"
#include "PTP.Group.pb.h"
#include "models/ModelGroup.h"

using namespace PTP::Common;

namespace ACTION_GROUP {
    void GroupRemoveSessionNotifyAction(CRequest* request){
        PTP::Group::GroupRemoveSessionNotify msg; 
        while (true){
            if(!msg.ParseFromArray(request->GetRequestPdu()->GetBodyData(), (int)request->GetRequestPdu()->GetBodyLength()))
            {
                break;
            }
            break;
        }
    }
};

