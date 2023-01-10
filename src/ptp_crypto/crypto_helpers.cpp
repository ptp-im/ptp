#include <iomanip>
#include <ctime>
#include <array>
#include "crypto_helpers.h"
#include "keccak.h"
#include "md5.h"
#include <openssl/rand.h>

using namespace std;

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

void gen_random_bytes(unsigned char *buf,int len) {
    RAND_bytes(buf,len);
}

string gen_password_by_salt(string strPass,string strSalt){
    string strInPass = strPass + strSalt;
    char szMd5[33];
    CMd5::MD5_Calculate(strInPass.c_str(), strInPass.length(), reinterpret_cast<string &>(szMd5));
    string strOutPass(szMd5);
    return strOutPass;
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
