/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyImportContacts.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __BUDDYIMPORTCONTACTS_H__
#define __BUDDYIMPORTCONTACTS_H__

#include "ImPduBase.h"

namespace COMMAND {

    void BuddyImportContactsReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    void BuddyImportContactsResCmd(CImPdu* pPdu);
};

#endif /*defined(__BUDDYIMPORTCONTACTS_H__) */
