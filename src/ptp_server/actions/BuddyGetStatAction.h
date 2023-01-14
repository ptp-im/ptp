/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyGetStatAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __BUDDYGETSTATACTION_H__
#define __BUDDYGETSTATACTION_H__

#include "../Request.h"
#include "../Response.h"

namespace ACTION_BUDDY {
    void BuddyGetStatReqAction(CRequest *request, CResponse *response);
    void BuddyGetStatResAction(CRequest* request, CResponse *response);
};

#endif /*defined(__BUDDYGETSTATACTION_H__) */
