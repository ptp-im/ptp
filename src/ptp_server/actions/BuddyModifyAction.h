/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyModifyAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __BUDDYMODIFYACTION_H__
#define __BUDDYMODIFYACTION_H__

#include "../Request.h"

namespace ACTION_BUDDY {
    void BuddyModifyReqAction(CRequest *request);
    void BuddyModifyResAction(CRequest* request);
};

#endif /*defined(__BUDDYMODIFYACTION_H__) */
