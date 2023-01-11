/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupChangeMemberNotify.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： 
 *
 #pragma once
================================================================*/
#ifndef __GROUPCHANGEMEMBERNOTIFY_H__
#define __GROUPCHANGEMEMBERNOTIFY_H__

#include "ptp_protobuf/ImPdu.h"

namespace COMMAND {
    void GroupChangeMemberNotifyCmd(CImPdu* pPdu);
};

#endif /*defined(__GROUPCHANGEMEMBERNOTIFY_H__) */
