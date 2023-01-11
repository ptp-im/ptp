/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgUnNotify.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： 
 *
 #pragma once
================================================================*/
#ifndef __MSGUNNOTIFY_H__
#define __MSGUNNOTIFY_H__

#include "ptp_protobuf/ImPdu.h"

namespace COMMAND {
    void MsgUnNotifyCmd(CImPdu* pPdu);
};

#endif /*defined(__MSGUNNOTIFY_H__) */
