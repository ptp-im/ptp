/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyGetStatAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __BUDDYGETSTATACTION_H__
#define __BUDDYGETSTATACTION_H__

#include "../Request.h"

namespace ACTION_BUDDY {
    void BuddyGetStatReqAction(CRequest *request);
    void BuddyGetStatResAction(CRequest* request);
};

#endif /*defined(__BUDDYGETSTATACTION_H__) */
