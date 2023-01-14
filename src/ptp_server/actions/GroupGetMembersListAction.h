/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupGetMembersListAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPGETMEMBERSLISTACTION_H__
#define __GROUPGETMEMBERSLISTACTION_H__

#include "Request.h"
#include "Response.h"

namespace ACTION_GROUP {
    void GroupGetMembersListReqAction(CReuest *request, CResponse *response);
    void GroupGetMembersListResAction(CRequest* request, CResponse *response);
};

#endif /*defined(__GROUPGETMEMBERSLISTACTION_H__) */
