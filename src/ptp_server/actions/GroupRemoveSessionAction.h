/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupRemoveSessionAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPREMOVESESSIONACTION_H__
#define __GROUPREMOVESESSIONACTION_H__

#include "../Request.h"

namespace ACTION_GROUP {
    void GroupRemoveSessionReqAction(CRequest *request);
    void GroupRemoveSessionResAction(CRequest* request);
};

#endif /*defined(__GROUPREMOVESESSIONACTION_H__) */
