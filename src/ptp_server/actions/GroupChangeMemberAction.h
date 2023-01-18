/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupChangeMemberAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPCHANGEMEMBERACTION_H__
#define __GROUPCHANGEMEMBERACTION_H__

#include "../Request.h"

namespace ACTION_GROUP {
    void GroupChangeMemberReqAction(CRequest *request);
    void GroupChangeMemberResAction(CRequest* request);
};

#endif /*defined(__GROUPCHANGEMEMBERACTION_H__) */
