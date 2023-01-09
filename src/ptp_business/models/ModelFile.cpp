/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelFile.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Saturday, November 5, 2022
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

void CModelFile::FileImgUploadReq(PTP::File::FileImgUploadReq* msg, PTP::File::FileImgUploadRes* msg_rsp, ERR& error)
{
    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn("group");

    while (true) {
        if (!pCacheConn) {
            error = PTP::Common::E_SYSTEM;
            log_error("error pCacheConn");
            break;
        }
        log("FileImgUploadReq...");
        // auto fromUserId = msg->auth_uid();
        break;
    }
    
    if (pCacheConn) {
        pCacheManager->RelCacheConn(pCacheConn);
    }
}

