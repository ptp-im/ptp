/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyQueryListAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __BUDDYQUERYLISTACTION_H__
#define __BUDDYQUERYLISTACTION_H__

#include "Request.h"
#include "Response.h"

namespace ACTION_BUDDY {
    void BuddyQueryListReqAction(CReuest *request, CResponse *response);
    void BuddyQueryListResAction(CRequest* request, CResponse *response);
};

#endif /*defined(__BUDDYQUERYLISTACTION_H__) */
