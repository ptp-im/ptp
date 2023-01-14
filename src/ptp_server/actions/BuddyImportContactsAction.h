/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyImportContactsAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __BUDDYIMPORTCONTACTSACTION_H__
#define __BUDDYIMPORTCONTACTSACTION_H__

#include "Request.h"
#include "Response.h"

namespace ACTION_BUDDY {
    void BuddyImportContactsReqAction(CReuest *request, CResponse *response);
    void BuddyImportContactsResAction(CRequest* request, CResponse *response);
};

#endif /*defined(__BUDDYIMPORTCONTACTSACTION_H__) */
