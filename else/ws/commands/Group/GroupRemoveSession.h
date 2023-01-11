/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupRemoveSession.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPREMOVESESSION_H__
#define __GROUPREMOVESESSION_H__

#include "ptp_protobuf/ImPdu.h"

namespace COMMAND {

    void GroupRemoveSessionReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    void GroupRemoveSessionResCmd(CImPdu* pPdu);
};

#endif /*defined(__GROUPREMOVESESSION_H__) */
