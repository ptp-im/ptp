#ifndef __ENCDEC_H__
#define __ENCDEC_H__

#include <iostream>
#include <openssl/aes.h>

using namespace std;
class CAes
{
public:
    CAes(const string& strKey);
    
    int Encrypt(const char* pInData, uint32_t nInLen, char** ppOutData, uint32_t& nOutLen);
    int Decrypt(const char* pInData, uint32_t nInLen, char** ppOutData, uint32_t& nOutLen);
    void Free(char* pData);
    
private:
    AES_KEY m_cEncKey;
    AES_KEY m_cDecKey;
    
};

#endif /*defined(__ENCDEC_H__) */