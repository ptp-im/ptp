/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyGetListAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __BUDDYGETLISTACTION_H__
#define __BUDDYGETLISTACTION_H__

#include "../Request.h"

namespace ACTION_BUDDY {
    void BuddyGetListReqAction(CRequest *request);
    void BuddyGetListResAction(CRequest* request);
};

#endif /*defined(__BUDDYGETLISTACTION_H__) */
