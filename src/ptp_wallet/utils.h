#ifndef BIP39_UTILS_HPP
#define BIP39_UTILS_HPP

#include <cassert>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <ptp_toolbox/data/bytes_array.h>
#include <ptp_toolbox/data/bytes_data.h>
#include <ptp_toolbox/data/utils.h>

#define MINTER_OK      0 /** Success */
#define MINTER_ERROR  -1 /** General error */
#define MINTER_EINVAL -2 /** Invalid argument */
#define MINTER_ENOMEM -3 /** malloc() failed */

#include <iostream>
#include <vector>
using namespace std;

namespace PTPWallet {

vector<string> split_string (string s, string delimiter);

uint32_t str_to_uint32_t(std::string const& str, size_t* idx = 0, int base = 10);
struct to_hmac_sha512;
using bytes_data = ptp_toolbox::data::bytes_data;
template<size_t N>
using bytes_array = ptp_toolbox::data::bytes_array<N>;
using bytes_64 = ptp_toolbox::data::bytes_array<64>;
using bytes_32 = ptp_toolbox::data::bytes_array<32>;
using bip32_key = ptp_toolbox::data::bytes_array<112>;

}

#endif