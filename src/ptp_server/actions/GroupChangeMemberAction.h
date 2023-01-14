/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupChangeMemberAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPCHANGEMEMBERACTION_H__
#define __GROUPCHANGEMEMBERACTION_H__

#include "../Request.h"
#include "../Response.h"

namespace ACTION_GROUP {
    void GroupChangeMemberReqAction(CRequest *request, CResponse *response);
    void GroupChangeMemberResAction(CRequest* request, CResponse *response);
};

#endif /*defined(__GROUPCHANGEMEMBERACTION_H__) */
