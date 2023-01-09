#include <iomanip>
#include <sstream>
#include <fcntl.h>
#include <iomanip>
#include <ctime>
#include <unistd.h>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#ifndef _WIN32
#include <strings.h>
#endif

#include <sys/stat.h>
#include <cassert>

#ifdef _WIN32
#define	snprintf	sprintf_s
#else
#include <ctime>
#include <sys/time.h>
#endif


using namespace std;

string bytes_to_hex_string(const uint8_t *str, uint64_t s);

string hex_to_string(const string &input);

uint32_t unix_timestamp();

uint64_t unix_timestamp_m();

uint64_t get_tick_count();
void util_sleep(uint32_t millisecond);
char* replaceStr(char* pSrc, char oldChar, char newChar);
string int2string(uint32_t user_id);
string int64ToString(uint64_t user_id);
uint32_t string2int(const string& value);
/**
 * @deprecated
 * @param str
 * @param new_value
 * @param begin_pos
 */
void replace_mark(string& str, string& new_value, uint32_t& begin_pos);
/**
 * @deprecated
 * @param str
 * @param new_value
 * @param begin_pos
 */
void replace_mark(string& str, uint32_t new_value, uint32_t& begin_pos);

void writePid();
void writePid(const string& name);
unsigned char toHex(const unsigned char &x);
unsigned char fromHex(const unsigned char &x);
string URLEncode(const string &sIn);
string URLDecode(const string &sIn);
//const char* memfind(const char *src_str,size_t src_len, const char *sub_str, size_t sub_len, bool flag = true);
int64_t get_file_size(const char *path);
void get_file_content(const char *path,char * fileBuf,uint64_t fileSize);

/**
 * @todo
 * @param str
 * @param new_value
 * @param begin_pos
 */
void put_file_content(const char *path,char * fileBuf,uint64_t fileSize);
