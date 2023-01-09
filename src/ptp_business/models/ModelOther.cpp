/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelOther.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Saturday, November 5, 2022
 *   desc：
 *
 *
================================================================*/
#include "ModelOther.h"
#include "CachePool.h"

CModelOther* CModelOther::m_pInstance = NULL;

CModelOther::CModelOther()
{
    
}

CModelOther::~CModelOther()
{
    
}

CModelOther* CModelOther::getInstance()
{
    if (m_pInstance == NULL) {
        m_pInstance = new CModelOther();
    }
    return m_pInstance;
}



