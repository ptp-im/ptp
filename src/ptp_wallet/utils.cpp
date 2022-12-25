#include "ptp_wallet/utils.h"
//#include "ptp_crypto/hmac_sha512.h"

#include <limits>
#include <stdexcept>

uint32_t PTPWallet::str_to_uint32_t(const std::string& str, size_t* idx, int base) {
    unsigned long result = std::stoul(str, idx, base);
    if (result > std::numeric_limits<uint32_t>::max()) {
        throw std::out_of_range("str_to_uint32_t");
    }
    return static_cast<uint32_t>(result);
}

vector<string> PTPWallet::split_string (string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}