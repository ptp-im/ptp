/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupChangeMemberNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc：
 *
================================================================*/
#include "GroupChangeMemberNotifyAction.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

namespace ACTION_GROUP {
    void GroupChangeMemberNotifyAction(CRequest* request, CResponse *response){
        PTP::Group::GroupChangeMemberNotify msg; 
        while (true){
            if(!msg.ParseFromArray(request->GetPdu()->GetBodyData(), (int)request->GetPdu()->GetBodyLength()))
            {
                break;
            }
            break;
        }
    }
};

