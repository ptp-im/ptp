/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupRemoveSessionAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPREMOVESESSIONACTION_H__
#define __GROUPREMOVESESSIONACTION_H__

#include "Request.h"
#include "Response.h"

namespace ACTION_GROUP {
    void GroupRemoveSessionReqAction(CReuest *request, CResponse *response);
    void GroupRemoveSessionResAction(CRequest* request, CResponse *response);
};

#endif /*defined(__GROUPREMOVESESSIONACTION_H__) */
