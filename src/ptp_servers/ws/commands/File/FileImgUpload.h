/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  FileImgUpload.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __FILEIMGUPLOAD_H__
#define __FILEIMGUPLOAD_H__

#include "ptp_global/ImPduBase.h"

namespace COMMAND {

    void FileImgUploadReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    void FileImgUploadResCmd(CImPdu* pPdu);
};

#endif /*defined(__FILEIMGUPLOAD_H__) */
