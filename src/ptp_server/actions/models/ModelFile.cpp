/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelFile.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
 *
================================================================*/
#include "ModelFile.h"
#include "CachePool.h"

CModelFile* CModelFile::m_pInstance = NULL;

CModelFile::CModelFile()
{
    
}

CModelFile::~CModelFile()
{
    
}

CModelFile* CModelFile::getInstance()
{
    if (m_pInstance == NULL) {
        m_pInstance = new CModelFile();
    }
    return m_pInstance;
}
