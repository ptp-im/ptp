/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelMsg.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
 *
================================================================*/
#include "ModelMsg.h"
#include "CachePool.h"

CModelMsg* CModelMsg::m_pInstance = NULL;

CModelMsg::CModelMsg()
{
    
}

CModelMsg::~CModelMsg()
{
    
}

CModelMsg* CModelMsg::getInstance()
{
    if (m_pInstance == NULL) {
        m_pInstance = new CModelMsg();
    }
    return m_pInstance;
}
