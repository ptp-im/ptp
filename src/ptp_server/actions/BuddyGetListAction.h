/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyGetListAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __BUDDYGETLISTACTION_H__
#define __BUDDYGETLISTACTION_H__

#include "../Request.h"
#include "../Response.h"

namespace ACTION_BUDDY {
    void BuddyGetListReqAction(CRequest *request, CResponse *response);
    void BuddyGetListResAction(CRequest* request, CResponse *response);
};

#endif /*defined(__BUDDYGETLISTACTION_H__) */
