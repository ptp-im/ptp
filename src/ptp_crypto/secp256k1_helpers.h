#include <sstream>
#include <secp256k1_recovery.h>

using namespace std;

string recover_address_from_sig_hex(string sig_hex, string msg);

string pub_key_to_address(string pubkey_64);

string recover_pub_key_from_sig(string sig, string msg_data);

string recover_address_from_sig(string _sig, string msg);

string recover_pub_key_from_sig_hex(string sig_hex, string msg);

void gen_ec_key_pair(secp256k1_context * ctx,unsigned char * prv_key, unsigned char * pub_key);

string recover_pub_key_from_sig_64(unsigned char * sig,int rec_id, string msg_data);

string recover_pub_key_from_sig_65(unsigned char * sig_65, string msg_data);

string recover_pub_key_from_sig(string sig, string msg_data);

void ecdsa_sign_recoverable(secp256k1_context *ctx,string msg_data,unsigned char * prv_key,unsigned char *output65);

bool recover_pub_key_from_sig_65(unsigned char * sig_65,string msg_data,unsigned char * pub_key,string& address);

bool ecdh_create_share_key(secp256k1_pubkey *pub_key,unsigned char *prv_key,unsigned char *shared_key);
bool ecdh_create_share_key(secp256k1_pubkey *pub_key,string &prv_key,unsigned char *shared_key);