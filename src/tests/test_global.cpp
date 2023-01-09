#include <gtest/gtest.h>
#include "ptp_global/global.h"
#include "ptp_global/Base64.h"
#include "ptp_global/Base64Utils.h"
#include "ptp_global/Logger.h"
#include "ptp_global/UtilPdu.h"
#include "ptp_global/Helpers.h"
#include "ptp_global/Timer.h"
#include "ptp_global/Util.h"

TEST(Timer, Timer) {
    Timer t;
    t.setInterval([&]() {
        cout << "Hey.. After each 1s..." << endl;
    }, 1000);
    t.setTimeout([&]() {
        cout << "Hey.. After 5.2s. But I will stop the timer!" << endl;
        t.stop();
    }, 5200);
    cout << "I am Timer" <<endl;
    while(true){
        if(t.isStopped()){
            cout << "Timer break" <<endl;
            break;
        }
    }
}

TEST(Util, Util_toHex) {
    const unsigned char c = 'c';
    DEBUG_D("char:%c",c);
    auto hex = toHex(c);
    DEBUG_D("hex:%x",hex);
    auto t1 = fromHex(hex);
    DEBUG_D("fromHex:%c",t1);
    ASSERT_EQ(t1,c);
}

TEST(Util, URLEncode) {
    string url = "htpp://www.google.com";
    DEBUG_D("url:%s",url.c_str());
    auto url_encode = URLEncode(url);
    DEBUG_D("URLEncode:%s",url_encode.c_str());
    auto url1 = URLDecode(url_encode);
    DEBUG_D("URLDecode:%s",url1.c_str());
    ASSERT_EQ(url,url1);
}

TEST(Util, replaceStr) {
//    char * str = "123456789";
//    char oldChar = '2';
//    char newChar = '3';
    //todo
    //auto res = replaceStr(str,oldChar,newChar);
    ASSERT_EQ(true,true);
}

TEST(Util, int2string) {
    int i = 1;
    string s = int2string(i);
    ASSERT_EQ(s,"1");
}

TEST(Util, int64ToString) {
    int64_t i = 1;
    string s = int64ToString(i);
    ASSERT_EQ(s,"1");
}

TEST(Util, string2int) {
    string s = "22";
    int64_t i = string2int(s);
    ASSERT_EQ(i,22);
}

TEST(Util, get_tick_count) {
    auto t = get_tick_count();
    DEBUG_D("get_tick_count:%ld",t);
    ASSERT_TRUE(t>0);
}

TEST(Util, util_sleep) {
    auto t = get_tick_count();
    DEBUG_D("get_tick_count :%ld",t);
    util_sleep(1000);
    auto t1 = get_tick_count();
    DEBUG_D("get_tick_count1:%ld",t1);
    DEBUG_D("sleep:%ld",t1-t);
    ASSERT_TRUE(t1-t>=1000);
}

TEST(test_global, CSimpleBuffer) {
    char const* str = "123456789";
    CSimpleBuffer m_buf = *new CSimpleBuffer();
    m_buf.Write((void *)str, strlen(str));
    uchar_t buff1[2];
    m_buf.ReadBuffer(buff1,2,0);
    std::string myString(reinterpret_cast<const char *>(buff1), 2);
    ASSERT_EQ(myString,"12");
    uchar_t buff2[4];
    m_buf.ReadBuffer(buff2,4,2);
    std::string myString1(reinterpret_cast<const char *>(buff2), 4);
    ASSERT_EQ(myString1,"3456");
    std::string myString2(reinterpret_cast<const char *>(m_buf.GetBuffer()+1), 4);
    ASSERT_EQ(myString2,"2345");
    std::string myString3(reinterpret_cast<const char *>(m_buf.GetBuffer()+2), 2);
    ASSERT_EQ(myString3,"34");
}

TEST(test_global, Base64) {
    string str = "1234&*%^!@#$%^&*()5678'/.,_||`~<>?:9#s";
    string t1 = base64_encode(str);
    DEBUG_D("base64_encode:%s",t1.c_str());
    string t12 = base64_decode(t1);
    DEBUG_D("base64_decode:%s",t12.c_str());
    ASSERT_EQ(str,t12);

    string t2 = base64_encode(str);
    DEBUG_D("base64_encode:%s",t2.c_str());
    string t21 = base64_decode(t2);
    DEBUG_D("base64_decode:%s",t21.c_str());
    ASSERT_EQ(str,t21);
    ASSERT_EQ(t12,t21);
}


TEST(Util, get_file_size) {
    DEBUG_D("__FILE__:%s",__FILE__);
    int64_t size = get_file_size(__FILE__);
    DEBUG_D("size:%d",size);
    ASSERT_TRUE(size > 0);
}

TEST(Util, get_file_content) {
    DEBUG_D("__FILE__:%s",__FILE__);
    int64_t size = get_file_size(__FILE__);
    char *buf = new char [size]();
    get_file_content(__FILE__,buf,size);
    DEBUG_D("size:%d",size);
    DEBUG_D("buf ====>>>\n%s",buf);
    ASSERT_TRUE(size > 0);
}

TEST(Util, put_file_content) {
    char const* path= "/tmp/put_file_content1.txt";
    string content= "a1234567";
    DEBUG_D("buf ====>>>\n%s",(char *)content.c_str());
    size_t size = content.size();
    DEBUG_D("size:%d",size);
    //todo
    ASSERT_TRUE(true);
//    put_file_content(path,(char *)content.c_str(), size);
//    int64_t size1 = get_file_size(path);
//    char *buf = new char [size1]();
//    get_file_content(path,buf,size1);
//    DEBUG_D("size1:%d",size1);
//    DEBUG_D("buf ====>>>\n%s",buf);
//    ASSERT_TRUE(size1 == size);
}
//
//TEST(Util, memfind) {
//    char const* src = "123456789";
//    char const* sub = "8";
//    auto res = memfind(src, strlen(src),sub, strlen(sub));
//    DEBUG_D("memfind:%s",res);
//    ASSERT_TRUE(string(res, strlen(res)) == "89");
//}


TEST(Util, writePid) {
    auto curPid = (uint32_t) getpid();
    DEBUG_D("curPid:%d",curPid);
    writePid();
    int64_t size = get_file_size("server.pid");
    char *buf = new char [size]();
    get_file_content("server.pid",buf,size);
    DEBUG_D("size:%d",size);
    DEBUG_D("buf ====>>>\n%s",buf);
    ASSERT_TRUE(string(buf, size) == to_string(curPid));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
