/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupCreateAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPCREATEACTION_H__
#define __GROUPCREATEACTION_H__

#include "Request.h"
#include "Response.h"

namespace ACTION_GROUP {
    void GroupCreateReqAction(CReuest *request, CResponse *response);
    void GroupCreateResAction(CRequest* request, CResponse *response);
};

#endif /*defined(__GROUPCREATEACTION_H__) */
