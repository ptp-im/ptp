/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelAuth.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Saturday, November 26, 2022
 *   desc：
 *
 *
================================================================*/
#include "ModelAuth.h"
#include "CachePool.h"

CModelAuth* CModelAuth::m_pInstance = NULL;

CModelAuth::CModelAuth()
{
    
}

CModelAuth::~CModelAuth()
{
    
}

CModelAuth* CModelAuth::getInstance()
{
    if (m_pInstance == NULL) {
        m_pInstance = new CModelAuth();
    }
    return m_pInstance;
}



