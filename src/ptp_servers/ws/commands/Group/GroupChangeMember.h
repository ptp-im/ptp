/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupChangeMember.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPCHANGEMEMBER_H__
#define __GROUPCHANGEMEMBER_H__

#include "ptp_global/ImPduBase.h"

namespace COMMAND {

    void GroupChangeMemberReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    void GroupChangeMemberResCmd(CImPdu* pPdu);
};

#endif /*defined(__GROUPCHANGEMEMBER_H__) */
