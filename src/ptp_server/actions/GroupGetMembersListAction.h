/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupGetMembersListAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPGETMEMBERSLISTACTION_H__
#define __GROUPGETMEMBERSLISTACTION_H__

#include "../Request.h"

namespace ACTION_GROUP {
    void GroupGetMembersListReqAction(CRequest *request);
    void GroupGetMembersListResAction(CRequest* request);
};

#endif /*defined(__GROUPGETMEMBERSLISTACTION_H__) */
