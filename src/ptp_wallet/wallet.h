//
// Created by jack on 2022/11/13.
//
#include <string>
#include <ptp_crypto/hmac_sha512.h>

#ifndef BD_MOBILE_CLIENT_WALLET_H
#define BD_MOBILE_CLIENT_WALLET_H

namespace PTPWallet {
    struct to_hmac_sha512 {
    public:
        to_hmac_sha512(const char* key)
                : m_key(reinterpret_cast<const unsigned char*>(key)),
                  m_key_len(strlen(key)) {
        }

        to_hmac_sha512(const uint8_t* key, size_t keylen)
                : m_key(key),
                  m_key_len(keylen) {
        }

        std::vector<uint8_t> operator()(const std::vector<uint8_t>& source) {
            std::vector<uint8_t> out(64);
            CHMAC_SHA512 hm(m_key, m_key_len);
            hm.Write(source.data(), source.size());
            hm.Finalize(&out[0]);
            return out;
        };
    private:
        const uint8_t* m_key;
        size_t m_key_len;
    };
}



#endif //BD_MOBILE_CLIENT_WALLET_H
