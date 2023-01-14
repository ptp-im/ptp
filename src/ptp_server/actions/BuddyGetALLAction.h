/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyGetALLAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __BUDDYGETALLACTION_H__
#define __BUDDYGETALLACTION_H__

#include "../Request.h"
#include "../Response.h"

namespace ACTION_BUDDY {
    void BuddyGetALLReqAction(CRequest *request, CResponse *response);
    void BuddyGetALLResAction(CRequest* request, CResponse *response);
};

#endif /*defined(__BUDDYGETALLACTION_H__) */
