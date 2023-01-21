#include <iomanip>
#include <sstream>
#include <fcntl.h>
#include <iomanip>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <list>
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
string bytes_to_hex_string(const unsigned char *str, uint64_t s);
string hex_to_string(const string &input);
uint32_t unix_timestamp();
uint64_t unix_timestamp_m();
uint64_t get_tick_count();
void util_sleep(uint32_t millisecond);
char* replaceStr(char* pSrc, char oldChar, char newChar);
int string_to_int(const string& value);
uint64_t string_to_uint64(const string& value);
void writePid();
void writePid(const string& name);
unsigned char toHex(const unsigned char &x);
unsigned char fromHex(const unsigned char &x);
string URLEncode(const string &sIn);
string URLDecode(const string &sIn);
bool file_exists(const char * name);
void make_dir(const char * dir);
void remove_file(const char * name);
int64_t get_file_size(const char *path);
void get_file_content(const char *path,char * fileBuf,uint64_t fileSize);
string get_file_name (const string& path);
string get_dir_path (const string& path);
void replace_string(string &config,const string &str,const string &replace_str);
/**
 * @todo
 * @param str
 * @param new_value
 * @param begin_pos
 */
void put_file_content(const char *path,char * fileBuf,uint64_t fileSize);

bool list_int_contains(list<uint32_t> &l,uint32_t i);
bool list_string_contains(list<string> &l,const string& s);