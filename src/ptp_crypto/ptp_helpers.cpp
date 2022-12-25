#include <iomanip>
#include <ctime>
#include <array>
#include <chrono>
#include "ptp_crypto/ptp_helpers.h"
#include "ptp_crypto/keccak.h"

using namespace std;

string bytes_to_hex_string(const uint8_t *str, uint64_t s) {
    ostringstream ret;
    for (size_t i = 0; i < s; ++i)
        ret << hex << setfill('0') << setw(2) << nouppercase << (int) str[i];
    return "0x" + ret.str();
}

string hex_to_string(const string &input) {
    static const char *const lut = "0123456789abcdef";
    size_t len = input.length();
    if (len & 1) throw invalid_argument("odd length");

    string output;
    output.reserve(len / 2);
    for (size_t i = 0; i < len; i += 2) {
        char a = input[i];
        const char *p = lower_bound(lut, lut + 16, a);
        if (*p != a) throw invalid_argument("not a hex digit");

        char b = input[i + 1];
        const char *q = lower_bound(lut, lut + 16, b);
        if (*q != b) throw invalid_argument("not a hex digit");

        output.push_back(((p - lut) << 4) | (q - lut));
    }
    return output;
}

uint32_t unix_timestamp(){
    return (uint32_t) time(NULL);
}

uint64_t unix_timestamp_m(){
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

string gen_random(int len) {
    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        int r = (time(nullptr)*rand() + rand()) % (sizeof(alphanum) - 1);
        tmp_s += alphanum[r];
    }
    return tmp_s;
}

array<uint8_t, 32> keccak_256_hash(string msg_data){
    array<uint8_t, 32> msg_hash;
    Keccak256::getHash(reinterpret_cast<const uint8_t *>(msg_data.data()), msg_data.length(), msg_hash.data());
    return msg_hash;
}

string pub_key_to_hex(unsigned char * pubkey_serialize){
    ostringstream pubkey_hex;
    for (size_t i = 1; i < 65; ++i){
        pubkey_hex  << hex << setfill('0') << setw(2) << nouppercase << (int) pubkey_serialize[i];
    }
//    log_debug("pubkey hex: 0x%s", pubkey_hex.str().c_str());
    return "0x" + pubkey_hex.str();
}

string address_to_hex(string address){
    return  bytes_to_hex_string(reinterpret_cast<const uint8_t *>(address.data()), address.length());
}

string format_eth_msg_data(const string& message){
    return string("\x19") + "Ethereum Signed Message:\n" + to_string(message.size()) + message;
}
