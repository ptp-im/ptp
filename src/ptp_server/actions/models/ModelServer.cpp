/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelServer.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
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
