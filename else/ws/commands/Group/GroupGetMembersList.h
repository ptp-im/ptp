/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupGetMembersList.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPGETMEMBERSLIST_H__
#define __GROUPGETMEMBERSLIST_H__

#include "ptp_protobuf/ImPdu.h"

namespace COMMAND {

    void GroupGetMembersListReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    void GroupGetMembersListResCmd(CImPdu* pPdu);
};

#endif /*defined(__GROUPGETMEMBERSLIST_H__) */
