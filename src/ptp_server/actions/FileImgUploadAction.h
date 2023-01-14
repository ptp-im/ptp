/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  FileImgUploadAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __FILEIMGUPLOADACTION_H__
#define __FILEIMGUPLOADACTION_H__

#include "Request.h"
#include "Response.h"

namespace ACTION_FILE {
    void FileImgUploadReqAction(CReuest *request, CResponse *response);
    void FileImgUploadResAction(CRequest* request, CResponse *response);
};

#endif /*defined(__FILEIMGUPLOADACTION_H__) */
