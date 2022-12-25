#include "ptp_wallet/MnemonicHelper.h"
#include "utils.h"

#include <openssl/rand.h>
#include <vector>
#include "word_list.h"

using namespace std;

std::vector<std::string> PTPWallet::MnemonicHelper::getLanguages() {
    int sz = bip39_get_languages_size();
    if (sz <= 0) {
        return std::vector<std::string>(0);
    }

    std::vector<std::string> languages(static_cast<size_t>(sz));
    bip39_get_languages(languages);

    return languages;
}
std::vector<std::string> PTPWallet::MnemonicHelper::getWordsFromLanguage(const char* lang) {
    words* wl[1];
    bip39_get_wordlist(lang, wl);
    if (!wl[0]) {
        return {};
    }

    std::vector<std::string> wordsList(wl[0]->len);
    for (size_t i = 0; i < wordsList.size(); i++) {
        wordsList[i] = std::string(wl[0]->indices[i]);
    }

    return wordsList;
}

PTPWallet::MnemonicHelper::MnemonicResult PTPWallet::MnemonicHelper::generate(const char* lang, size_t entropy_len) {
    assert(BIP39_ENTROPY_LEN_128 == entropy_len);
    unsigned char buf[entropy_len];
    RAND_bytes(buf, (int) entropy_len);
    return encodeBytes(buf, lang, entropy_len);
}

PTPWallet::MnemonicHelper::MnemonicResult PTPWallet::MnemonicHelper::encodeBytes(const uint8_t* src,
                                                                                 const char* lang,
                                                                                 size_t entropy_len) {
    assert(BIP39_ENTROPY_LEN_128 == entropy_len);
    MnemonicResult result{Ok, std::vector<std::string>(0), "", 0};

    if (!validateEntropy(entropy_len)) {
        result.status = UnsupportedEntropy;
        return result;
    }

    struct words* wordList[1];
    bip39_get_wordlist(lang, wordList);

    char* output[1];
    bool encRes = bip39_mnemonic_from_bytes(wordList[0], src, entropy_len, output) == MINTER_OK;

    if (!encRes) {
        result.status = UnknownError;
        return result;
    }

    result.words = split_string(output[0], " ");
    result.len = result.words.size();
    result.raw = std::string(output[0]);

    free(output[0]);

    return result;
}

PTPWallet::MnemonicHelper::MnemonicResult PTPWallet::MnemonicHelper::entropyHexToMnemonic(const string& entropyHex, const char* lang) {
    auto entropyBytes = ptp_toolbox::data::hex_to_bytes(entropyHex.data());
    return entropyToMnemonic(entropyBytes.data(),"en",16);
}

PTPWallet::MnemonicHelper::MnemonicResult PTPWallet::MnemonicHelper::entropyToMnemonic(const uint8_t* entropyBytes, const char* lang, size_t entropyLen) {
    MnemonicResult result{Ok, std::vector<std::string>(0), "", 0};

    if (!validateEntropy(entropyLen)) {
        result.status = UnsupportedEntropy;
        return result;
    }

    struct words* wordList[1];
    bip39_get_wordlist(lang, wordList);

    char* output[1];
    bool encRes = bip39_mnemonic_from_bytes(wordList[0], entropyBytes, entropyLen, output) == MINTER_OK;

    if (!encRes) {
        result.status = UnknownError;
        return result;
    }

    result.words = split_string(output[0], " ");
    result.len = result.words.size();
    result.raw = std::string(output[0]);

    free(output[0]);

    return result;
}
size_t PTPWallet::MnemonicHelper::decodeMnemonic(const char* mnemonicWords, const char* lang, uint8_t * mnemonicBytesOut, size_t entropyLen) {
    struct words* wordList[1];
    bip39_get_wordlist(lang, wordList);

    if (!validateEntropy(entropyLen)) {
        return false;
    }

    size_t written = 0;
    bool decRes = bip39_mnemonic_to_bytes(wordList[0], mnemonicWords, mnemonicBytesOut, entropyLen, &written) == MINTER_OK;
    if (decRes) {
        return written;
    }
    return written;
}

size_t PTPWallet::MnemonicHelper::mnemonicWordsToEntropy(const char* mnemonicWords, const char* lang, uint8_t * mnemonicBytesOut, size_t entropyLen) {
    return decodeMnemonic(mnemonicWords,lang,mnemonicBytesOut,entropyLen);
}

void PTPWallet::MnemonicHelper::wordsToSeed(const char* words, uint8_t* out64, size_t* writtenSz) {
    bip39_mnemonic_to_seed(words, nullptr, out64, 64, writtenSz);
}

bool PTPWallet::MnemonicHelper::validateEntropy(size_t entropy) {
    switch (entropy) {
        case BIP39_ENTROPY_LEN_128:
        case BIP39_ENTROPY_LEN_160:
        case BIP39_ENTROPY_LEN_192:
        case BIP39_ENTROPY_LEN_224:
        case BIP39_ENTROPY_LEN_256:
        case BIP39_ENTROPY_LEN_288:
        case BIP39_ENTROPY_LEN_320:
            return true;
        default:
            break;
    }

    return false;
}

bool PTPWallet::MnemonicHelper::validateWords(const char* lang, const char* mnemonicWords) {
    words* wl[1];
    bip39_get_wordlist(lang, wl);
    if (!wl[0]) {
        return false;
    }

    return bip39_mnemonic_validate(wl[0], mnemonicWords) == MINTER_OK;
}
