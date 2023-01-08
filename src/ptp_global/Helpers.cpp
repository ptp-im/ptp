#include "Helpers.h"

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

string int2string(uint32_t user_id)
{
    stringstream ss;
    ss << user_id;
    return ss.str();
}

string int64ToString(uint64_t user_id)
{
    stringstream ss;
    ss << user_id;
    return ss.str();
}

uint32_t string2int(const string& value)
{
    return (uint32_t)atoi(value.c_str());
}

// 由于被替换的内容可能包含?号，所以需要更新开始搜寻的位置信息来避免替换刚刚插入的?号
void replace_mark(string& str, string& new_value, uint32_t& begin_pos)
{
    string::size_type pos = str.find('?', begin_pos);
    if (pos == string::npos) {
        return;
    }

    string prime_new_value = "'"+ new_value + "'";
    str.replace(pos, 1, prime_new_value);

    begin_pos = pos + prime_new_value.size();
}

void replace_mark(string& str, uint32_t new_value, uint32_t& begin_pos)
{
    stringstream ss;
    ss << new_value;

    string str_value = ss.str();
    string::size_type pos = str.find('?', begin_pos);
    if (pos == string::npos) {
        return;
    }

    str.replace(pos, 1, str_value);
    begin_pos = pos + str_value.size();
}


void writePid()
{
    uint32_t curPid;
#ifdef _WIN32
    curPid = (uint32_t) GetCurrentProcess();
#else
    curPid = (uint32_t) getpid();
#endif
    FILE* f = fopen("server.pid", "w");
    assert(f);
    char szPid[32];
    snprintf(szPid, sizeof(szPid), "%d", curPid);
    fwrite(szPid, strlen(szPid), 1, f);
    fclose(f);
}

inline unsigned char toHex(const unsigned char &x)
{
    return x > 9 ? x -10 + 'A': x + '0';
}

inline unsigned char fromHex(const unsigned char &x)
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

void put_file_content(const char *path,char * fileBuf,uint64_t fileSize){
    int flags = O_RDWR | O_CREAT | O_EXCL;
    int m_file = open(path, flags);
    if(m_file){
        pwrite(m_file, fileBuf, fileSize, 0);
        close(m_file);
    }
}

const char* memfind(const char *src_str,size_t src_len, const char *sub_str, size_t sub_len, bool flag)
{
    if(NULL == src_str || NULL == sub_str || src_len <= 0)
    {
        return NULL;
    }
    if(src_len < sub_len)
    {
        return NULL;
    }
    const char *p;
    if (sub_len == 0)
        sub_len = strlen(sub_str);
    if(src_len == sub_len)
    {
        if(0 == (memcmp(src_str, sub_str, src_len)))
        {
            return src_str;
        }
        else
        {
            return NULL;
        }
    }
    if(flag)
    {
        for (int i = 0; i < src_len - sub_len; i++)
        {
            p = src_str + i;
            if(0 == memcmp(p, sub_str, sub_len))
                return p;
        }
    }
    else
    {
        for ( int i = (src_len - sub_len) ; i >= 0;i--  )
        {
            p = src_str + i;
            if ( 0 == memcmp(  p,sub_str,sub_len ) )
                return p;

        }
    }
    return NULL;
}