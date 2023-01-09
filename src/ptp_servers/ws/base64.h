#include <string>

std::basic_string<char, std::char_traits<char>, std::allocator<char>> base64_encode2(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);
