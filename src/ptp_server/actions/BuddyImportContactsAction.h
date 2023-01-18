/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyImportContactsAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __BUDDYIMPORTCONTACTSACTION_H__
#define __BUDDYIMPORTCONTACTSACTION_H__

#include "../Request.h"

namespace ACTION_BUDDY {
    void BuddyImportContactsReqAction(CRequest *request);
    void BuddyImportContactsResAction(CRequest* request);
};

#endif /*defined(__BUDDYIMPORTCONTACTSACTION_H__) */
