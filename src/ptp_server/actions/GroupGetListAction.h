/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupGetListAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPGETLISTACTION_H__
#define __GROUPGETLISTACTION_H__

#include "../Request.h"
#include "../Response.h"

namespace ACTION_GROUP {
    void GroupGetListReqAction(CRequest *request, CResponse *response);
    void GroupGetListResAction(CRequest* request, CResponse *response);
};

#endif /*defined(__GROUPGETLISTACTION_H__) */
