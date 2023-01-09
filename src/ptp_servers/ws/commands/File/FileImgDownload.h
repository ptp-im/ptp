/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  FileImgDownload.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __FILEIMGDOWNLOAD_H__
#define __FILEIMGDOWNLOAD_H__

#include "ImPduBase.h"

namespace COMMAND {

    void FileImgDownloadReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    void FileImgDownloadResCmd(CImPdu* pPdu);
};

#endif /*defined(__FILEIMGDOWNLOAD_H__) */
