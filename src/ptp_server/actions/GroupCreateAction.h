/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupCreateAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPCREATEACTION_H__
#define __GROUPCREATEACTION_H__

#include "../Request.h"

namespace ACTION_GROUP {
    void GroupCreateReqAction(CRequest *request);
    void GroupCreateResAction(CRequest* request);
};

#endif /*defined(__GROUPCREATEACTION_H__) */
