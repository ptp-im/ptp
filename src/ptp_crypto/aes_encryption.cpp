#include <iomanip>
#include <openssl/evp.h>
#include "ptp_crypto/aes_encryption.h"

int aes_gcm_encrypt(const unsigned char *plaintext, int plaintext_len,
            const unsigned char *key, const unsigned char *iv,
                    const unsigned char *aad,
            unsigned char *ciphertext)
{
    unsigned char outBuf[1024000];
    unsigned char tag[16] = { 0 };
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;
    int ret;

    if (!(ctx = EVP_CIPHER_CTX_new())){
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL)){
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 16, NULL)){
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    if (!EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv)){
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    if(nullptr != aad ){
        if (!EVP_EncryptUpdate (ctx, NULL, &len, aad, 16)) {
            EVP_CIPHER_CTX_free(ctx);
            return -1;
        }
    }
    if (!EVP_EncryptUpdate(ctx, outBuf, &len, plaintext, plaintext_len)){
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    ciphertext_len = len;

    ret = EVP_EncryptFinal_ex(ctx, outBuf + len, &len);

    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag)){
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    EVP_CIPHER_CTX_free(ctx);

    if (ret > 0)
    {
        ciphertext_len += len;
        std::copy_n(outBuf, ciphertext_len, tag + 16);
        ciphertext_len += 16;
        memcpy(ciphertext,tag,ciphertext_len);
        return ciphertext_len;
    }
    else
    {
        return -1;
    }
}

int aes_gcm_decrypt(const unsigned char *ciphertext, int ciphertext_len,
            const unsigned char *key, const unsigned char *iv,
                    const unsigned char *aad,
            unsigned char *plaintext)
{
    unsigned char tag[16] = { 0 };
    memcpy(tag,ciphertext,16);

    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
    int ret;

    if (!(ctx = EVP_CIPHER_CTX_new())){
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL)){
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 16, NULL)){
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    if (!EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv)){
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, (void*)tag)){
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    if(nullptr != aad){
        if (!EVP_DecryptUpdate(ctx, NULL, &len, aad, sizeof(aad))){
            EVP_CIPHER_CTX_free(ctx);
            return -1;
        }
    }

    if (!EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext + 16, ciphertext_len-16)){
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    plaintext_len = len;

    ret = EVP_DecryptFinal_ex(ctx, plaintext + len, &len);

    EVP_CIPHER_CTX_free(ctx);

    if (ret >= 0)
    {
        plaintext_len += len;
        return plaintext_len;
    }
    else
    {
        return -1;
    }
}