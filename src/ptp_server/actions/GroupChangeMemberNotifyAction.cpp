/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupChangeMemberNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "GroupChangeMemberNotifyAction.h"
#include "PTP.Group.pb.h"
#include "models/ModelGroup.h"

using namespace PTP::Common;

namespace ACTION_GROUP {
    void GroupChangeMemberNotifyAction(CRequest* request){
        PTP::Group::GroupChangeMemberNotify msg; 
        while (true){
            if(!msg.ParseFromArray(request->GetRequestPdu()->GetBodyData(), (int)request->GetRequestPdu()->GetBodyLength()))
            {
                break;
            }
            break;
        }
    }
};

