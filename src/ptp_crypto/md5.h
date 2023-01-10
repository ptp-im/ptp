//
// Created by jack on 2023/1/10.
//

#ifndef PTP_MD5_H
#define PTP_MD5_H
#include <iostream>
#include <openssl/md5.h>

class CMd5
{
public:
    static void MD5_Calculate (const char* pContent, unsigned int nLen,std::string& md5);
};

#endif //PTP_MD5_H
