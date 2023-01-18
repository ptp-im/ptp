/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupModifyNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "GroupModifyNotifyAction.h"
#include "PTP.Group.pb.h"
#include "models/ModelGroup.h"

using namespace PTP::Common;

namespace ACTION_GROUP {
    void GroupModifyNotifyAction(CRequest* request){
        PTP::Group::GroupModifyNotify msg; 
        while (true){
            if(!msg.ParseFromArray(request->GetRequestPdu()->GetBodyData(), (int)request->GetRequestPdu()->GetBodyLength()))
            {
                break;
            }
            break;
        }
    }
};

