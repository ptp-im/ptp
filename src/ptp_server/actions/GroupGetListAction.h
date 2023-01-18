/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupGetListAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPGETLISTACTION_H__
#define __GROUPGETLISTACTION_H__

#include "../Request.h"

namespace ACTION_GROUP {
    void GroupGetListReqAction(CRequest *request);
    void GroupGetListResAction(CRequest* request);
};

#endif /*defined(__GROUPGETLISTACTION_H__) */
