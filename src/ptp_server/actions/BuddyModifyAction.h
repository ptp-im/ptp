/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyModifyAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __BUDDYMODIFYACTION_H__
#define __BUDDYMODIFYACTION_H__

#include "../Request.h"
#include "../Response.h"

namespace ACTION_BUDDY {
    void BuddyModifyReqAction(CRequest *request, CResponse *response);
    void BuddyModifyResAction(CRequest* request, CResponse *response);
};

#endif /*defined(__BUDDYMODIFYACTION_H__) */
