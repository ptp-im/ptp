/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupRemoveSessionNotify.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： 
 *
 #pragma once
================================================================*/
#ifndef __GROUPREMOVESESSIONNOTIFY_H__
#define __GROUPREMOVESESSIONNOTIFY_H__

#include "ptp_protobuf/ImPdu.h"

namespace COMMAND {
    void GroupRemoveSessionNotifyCmd(CImPdu* pPdu);
};

#endif /*defined(__GROUPREMOVESESSIONNOTIFY_H__) */
