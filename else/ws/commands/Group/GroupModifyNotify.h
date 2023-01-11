/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupModifyNotify.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： 
 *
 #pragma once
================================================================*/
#ifndef __GROUPMODIFYNOTIFY_H__
#define __GROUPMODIFYNOTIFY_H__

#include "ptp_protobuf/ImPdu.h"

namespace COMMAND {
    void GroupModifyNotifyCmd(CImPdu* pPdu);
};

#endif /*defined(__GROUPMODIFYNOTIFY_H__) */
