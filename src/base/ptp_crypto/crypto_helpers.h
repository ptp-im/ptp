#include <iomanip>
#include <sstream>

using namespace std;

string bytes_to_hex_string(const unsigned char *str, uint64_t s);

string hex_to_string(const string &input);

uint32_t unix_timestamp();

uint64_t unix_timestamp_m();

string gen_random(int len);

void gen_random_bytes(unsigned char *buf,int len);

string gen_password_by_salt(string strPass,string strSalt);

array<uint8_t, 32> keccak_256_hash(string msg_data);

string pub_key_to_hex(unsigned char * pubkey_serialize);

string address_to_hex(string address);

string format_eth_msg_data(const string& message);
