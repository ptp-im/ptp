/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupChangeMemberNotifyAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： 
 *
 #pragma once
================================================================*/
#ifndef __GROUPCHANGEMEMBERNOTIFYACTION_H__
#define __GROUPCHANGEMEMBERNOTIFYACTION_H__

#include "../Request.h"
#include "../Response.h"

namespace ACTION_GROUP {
    void GroupChangeMemberNotifyAction(CRequest* request, CResponse *response);
};

#endif /*defined(__GROUPCHANGEMEMBERNOTIFYACTION_H__) */
