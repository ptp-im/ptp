/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelSwitch.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, November 6, 2022
 *   desc：
 *
 *
================================================================*/
#include "ModelSwitch.h"
#include "CachePool.h"

CModelSwitch* CModelSwitch::m_pInstance = NULL;

CModelSwitch::CModelSwitch()
{
    
}

CModelSwitch::~CModelSwitch()
{
    
}

CModelSwitch* CModelSwitch::getInstance()
{
    if (m_pInstance == NULL) {
        m_pInstance = new CModelSwitch();
    }
    return m_pInstance;
}


