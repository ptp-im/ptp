#include "Helpers.h"
#include <fstream>
#include <algorithm>

using namespace std;

string bytes_to_hex_string(const unsigned char *str, uint64_t s) {
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


uint64_t get_tick_count()
{
#ifdef _WIN32
    LARGE_INTEGER liCounter;
	LARGE_INTEGER liCurrent;

	if (!QueryPerformanceFrequency(&liCounter))
		return GetTickCount();

	QueryPerformanceCounter(&liCurrent);
	return (uint64_t)(liCurrent.QuadPart * 1000 / liCounter.QuadPart);
#else
    struct timeval tval;
    uint64_t ret_tick;

    gettimeofday(&tval, NULL);

    ret_tick = tval.tv_sec * 1000L + tval.tv_usec / 1000L;
    return ret_tick;
#endif
}

void util_sleep(uint32_t millisecond)
{
#ifdef _WIN32
    Sleep(millisecond);
#else
    usleep(millisecond * 1000);
#endif
}

char* replaceStr(char* pSrc, char oldChar, char newChar)
{
    if(NULL == pSrc)
    {
        return NULL;
    }

    char *pHead = pSrc;
    while (*pHead != '\0') {
        if(*pHead == oldChar)
        {
            *pHead = newChar;
        }
        ++pHead;
    }
    return pSrc;
}

int string_to_int(const string& value)
{
    return stoi(value);
}

uint64_t string_to_uint64(const string& value)
{
    return stoul(value);
}

void writePid(){
    writePid("server");
}

void writePid(const string &name)
{
    uint32_t curPid;

#ifdef _WIN32
    curPid = (uint32_t) GetCurrentProcess();
#else
    curPid = (uint32_t) getpid();
#endif

    string path;
    path.append("/tmp/");
    path.append(name);
    path.append(".pid");
    FILE* f = fopen(path.c_str(), "w");
    assert(f);
    char szPid[32];
    snprintf(szPid, sizeof(szPid), "%d", curPid);
    fwrite(szPid, strlen(szPid), 1, f);
    fclose(f);
}

unsigned char toHex(const unsigned char &x)
{
    return x > 9 ? x -10 + 'A': x + '0';
}

unsigned char fromHex(const unsigned char &x)
{
    return isdigit(x) ? x-'0' : x-'A'+10;
}

string URLEncode(const string &sIn)
{
    string sOut;
    for( size_t ix = 0; ix < sIn.size(); ix++ )
    {
        unsigned char buf[4];
        memset( buf, 0, 4 );
        if( isalnum( (unsigned char)sIn[ix] ) )
        {
            buf[0] = sIn[ix];
        }
            //else if ( isspace( (unsigned char)sIn[ix] ) ) //貌似把空格编码成%20或者+都可以
            //{
            //    buf[0] = '+';
            //}
        else
        {
            buf[0] = '%';
            buf[1] = toHex( (unsigned char)sIn[ix] >> 4 );
            buf[2] = toHex( (unsigned char)sIn[ix] % 16);
        }
        sOut += (char *)buf;
    }
    return sOut;
}

string URLDecode(const string &sIn)
{
    string sOut;
    for( size_t ix = 0; ix < sIn.size(); ix++ )
    {
        unsigned char ch = 0;
        if(sIn[ix]=='%')
        {
            ch = (fromHex(sIn[ix+1])<<4);
            ch |= fromHex(sIn[ix+2]);
            ix += 2;
        }
        else if(sIn[ix] == '+')
        {
            ch = ' ';
        }
        else
        {
            ch = sIn[ix];
        }
        sOut += (char)ch;
    }
    return sOut;
}

bool file_exists(const char * name) {
    struct stat buffer{};
    return (stat (name, &buffer) == 0);
}

void make_dir(const char * dir) {
    char tmp[256];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp),"%s",dir);
    len = strlen(tmp);
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for (p = tmp + 1; *p; p++)
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }
    mkdir(tmp, S_IRWXU);
}

int64_t get_file_size(const char *path)
{
    int64_t filesize = -1;
    struct stat statBuff{};
    if(stat(path, &statBuff) < 0){
        return filesize;
    }else{
        filesize = statBuff.st_size;
    }
    return filesize;
}

void get_file_content(const char *path,char * fileBuf,uint64_t fileSize){
    int m_file = open(path, O_RDONLY);
    if(m_file){
        pread(m_file, fileBuf, fileSize, 0);
        fileBuf[fileSize] = '\0';
        close(m_file);
    }
}

void remove_file(const char * name){
    remove(name);
}

string get_file_name(const string& path)
{
    size_t found;
    found=path.find_last_of("/\\");
    return path.substr(found+1);
}

string get_dir_path (const string& path)
{
    size_t found;
    found=path.find_last_of("/\\");
    return path.substr(0,found);
}
void put_file_content(const char *path,char * fileBuf,uint64_t fileSize){
    ofstream my_file;
    my_file.open (path);
    my_file << fileBuf;
    my_file.close();
}

void replace_string(string &str,const string &org_str,const string &replace_str){
    auto index = str.find(org_str,0);
    if(index != string::npos){
        str.replace(index,org_str.size(),replace_str);
    }
}

bool list_int_contains(list<uint32_t> &l,uint32_t i){
    auto it = std::find(l.begin(), l.end(), i);
    return it != l.end();
}

bool list_string_contains(list<string> &l,const string& s){
    auto it = std::find(l.begin(), l.end(), s);
    return it != l.end();
}