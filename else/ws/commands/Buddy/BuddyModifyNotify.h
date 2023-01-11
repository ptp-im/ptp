/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyModifyNotify.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： 
 *
 #pragma once
================================================================*/
#ifndef __BUDDYMODIFYNOTIFY_H__
#define __BUDDYMODIFYNOTIFY_H__

#include "ptp_protobuf/ImPdu.h"

namespace COMMAND {
    void BuddyModifyNotifyCmd(CImPdu* pPdu);
};

#endif /*defined(__BUDDYMODIFYNOTIFY_H__) */
