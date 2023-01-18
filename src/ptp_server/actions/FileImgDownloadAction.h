/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  FileImgDownloadAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __FILEIMGDOWNLOADACTION_H__
#define __FILEIMGDOWNLOADACTION_H__

#include "../Request.h"

namespace ACTION_FILE {
    void FileImgDownloadReqAction(CRequest *request);
    void FileImgDownloadResAction(CRequest* request);
};

#endif /*defined(__FILEIMGDOWNLOADACTION_H__) */
