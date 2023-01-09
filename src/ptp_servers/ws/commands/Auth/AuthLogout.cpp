/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  AuthLogout.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Saturday, November 5, 2022
 *   desc：
 *
================================================================*/

#include "util.h"
#include "ImPduBase.h"
#include "ImUser.h"
#include "AttachData.h"
#include "PTP.Auth.pb.h"

using namespace PTP::Common;

namespace COMMAND {
    void AuthLogoutReqCmd(CImPdu* pPdu, uint32_t conn_uuid){
        log_debug("AuthLogoutReq start...");
        auto pMsgConn = FindWebSocketConnByHandle(conn_uuid);
        if(!pMsgConn){
            log_error("not found pMsgConn");
            return;
        }
        pMsgConn->Close();
    }
};

