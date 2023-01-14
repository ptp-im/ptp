/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  FileImgDownloadAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __FILEIMGDOWNLOADACTION_H__
#define __FILEIMGDOWNLOADACTION_H__

#include "Request.h"
#include "Response.h"

namespace ACTION_FILE {
    void FileImgDownloadReqAction(CReuest *request, CResponse *response);
    void FileImgDownloadResAction(CRequest* request, CResponse *response);
};

#endif /*defined(__FILEIMGDOWNLOADACTION_H__) */
