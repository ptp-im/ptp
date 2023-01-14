/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupUnreadMsgAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPUNREADMSGACTION_H__
#define __GROUPUNREADMSGACTION_H__

#include "Request.h"
#include "Response.h"

namespace ACTION_GROUP {
    void GroupUnreadMsgReqAction(CReuest *request, CResponse *response);
    void GroupUnreadMsgResAction(CRequest* request, CResponse *response);
};

#endif /*defined(__GROUPUNREADMSGACTION_H__) */
