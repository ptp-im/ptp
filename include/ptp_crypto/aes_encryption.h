#ifndef AES_ENCRYPTION_H
#define AES_ENCRYPTION_H


int aes_gcm_encrypt(const unsigned char *plaintext, int plaintext_len,
            const unsigned char *key, const unsigned char *iv,
                    const unsigned char *aad,
            unsigned char *ciphertext);


int aes_gcm_decrypt(const unsigned char *ciphertext, int ciphertext_len,
            const unsigned char *key, const unsigned char *iv,
                    const unsigned char *aad,
            unsigned char *plaintext);

#endif // !AES_ENCRYPTION_H