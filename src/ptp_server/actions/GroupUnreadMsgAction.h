/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupUnreadMsgAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPUNREADMSGACTION_H__
#define __GROUPUNREADMSGACTION_H__

#include "../Request.h"

namespace ACTION_GROUP {
    void GroupUnreadMsgReqAction(CRequest *request);
    void GroupUnreadMsgResAction(CRequest* request);
};

#endif /*defined(__GROUPUNREADMSGACTION_H__) */
