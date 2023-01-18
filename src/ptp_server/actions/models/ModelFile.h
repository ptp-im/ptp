/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelFile.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：DO NOT EDIT!!
 *
 *
 ================================================================*/

#ifndef MODEL_FILE_H_
#define MODEL_FILE_H_

#include "PTP.File.pb.h"

using namespace std;
using namespace PTP::Common;

class CModelFile {
public:
virtual ~CModelFile();
    static CModelFile* getInstance();
private:
    CModelFile();
private:
    static CModelFile*    m_pInstance;
};

#endif /* MODEL_FILE_H_ */
