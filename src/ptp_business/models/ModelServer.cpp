/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelServer.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Saturday, November 5, 2022
 *   desc：
 *
 *
================================================================*/
#include "ModelServer.h"
#include "CachePool.h"

CModelServer* CModelServer::m_pInstance = NULL;

CModelServer::CModelServer()
{
    
}

CModelServer::~CModelServer()
{
    
}

CModelServer* CModelServer::getInstance()
{
    if (m_pInstance == NULL) {
        m_pInstance = new CModelServer();
    }
    return m_pInstance;
}

void CModelServer::ServerLoginReq(PTP::Server::ServerLoginReq* msg, PTP::Server::ServerLoginRes* msg_rsp, ERR& error)
{
    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn("group");

    while (true) {
        if (!pCacheConn) {
            error = PTP::Common::E_SYSTEM;
            log_error("error pCacheConn");
            break;
        }
        log("ServerLoginReq...");
        // auto fromUserId = msg->auth_uid();
        break;
    }
    
    if (pCacheConn) {
        pCacheManager->RelCacheConn(pCacheConn);
    }
}

