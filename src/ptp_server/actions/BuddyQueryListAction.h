/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyQueryListAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __BUDDYQUERYLISTACTION_H__
#define __BUDDYQUERYLISTACTION_H__

#include "../Request.h"

namespace ACTION_BUDDY {
    void BuddyQueryListReqAction(CRequest *request);
    void BuddyQueryListResAction(CRequest* request);
};

#endif /*defined(__BUDDYQUERYLISTACTION_H__) */
