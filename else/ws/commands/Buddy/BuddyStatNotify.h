/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyStatNotify.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： 
 *
 #pragma once
================================================================*/
#ifndef __BUDDYSTATNOTIFY_H__
#define __BUDDYSTATNOTIFY_H__

#include "ptp_protobuf/ImPdu.h"

namespace COMMAND {
    void BuddyStatNotifyCmd(CImPdu* pPdu);
};

#endif /*defined(__BUDDYSTATNOTIFY_H__) */
