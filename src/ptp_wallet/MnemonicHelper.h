#ifndef BIP39_BIP32MNEMONIC_H
#define BIP39_BIP32MNEMONIC_H

#include <iostream>
#include <random>
#include <vector>
#include "ptp_wallet/bip39.h"

namespace PTPWallet {

    class MnemonicHelper {
    public:
        enum MnemonicStatus {
            Ok,
            UnsupportedEntropy,
            UnknownError
        };

        struct MnemonicResult {
            MnemonicStatus status;
            std::vector<std::string> words;
            std::string raw;
            size_t len;
        };

        static std::vector<std::string> getLanguages();
        static std::vector<std::string> getWordsFromLanguage(const char* lang);
        static MnemonicResult generate(const char* lang = "en", size_t entropy_len = BIP39_ENTROPY_LEN_128);
        static MnemonicResult encodeBytes(const uint8_t* src,
                                          const char* lang = "en",
                                          size_t entropy_len = BIP39_ENTROPY_LEN_128);
        static size_t decodeMnemonic(const char* mnemonic, const char* lang, uint8_t * mnemonicBytesOut,size_t entropy_len = BIP39_ENTROPY_LEN_128);
        static size_t mnemonicWordsToEntropy(const char* mnemonicWords, const char* lang,uint8_t * mnemonicBytesOut,size_t entropyLen);
        static MnemonicResult entropyToMnemonic(const uint8_t* entropyBytes, const char* lang, size_t entropyLen);
        static MnemonicResult entropyHexToMnemonic(const string& entropyHex, const char* lang);
        static void wordsToSeed(const char* words, uint8_t* out64, size_t* writtenSz);
        static bool validateWords(const char* lang, const char* mnemonicWords);

    private:
        static bool validateEntropy(size_t entropy);
    };

}
#endif
