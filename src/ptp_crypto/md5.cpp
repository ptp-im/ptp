//
// Created by jack on 2023/1/10.
//

#include "md5.h"
#include "ptp_global/Helpers.h"

void CMd5::MD5_Calculate(const char *pContent, unsigned int nLen, std::string& md5)
{
    unsigned char d[16];
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, pContent, nLen);
    MD5_Final(d, &ctx);
    md5 = bytes_to_hex_string(d,sizeof d);
}
