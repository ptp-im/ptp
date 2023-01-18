/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  FileImgUploadAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __FILEIMGUPLOADACTION_H__
#define __FILEIMGUPLOADACTION_H__

#include "../Request.h"

namespace ACTION_FILE {
    void FileImgUploadReqAction(CRequest *request);
    void FileImgUploadResAction(CRequest* request);
};

#endif /*defined(__FILEIMGUPLOADACTION_H__) */
