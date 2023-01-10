#include <gtest/gtest.h>
#include "ptp_net/BuffersStorage.h"
#include "ptp_global/UtilPdu.h"
#include "ptp_global/SoketClient.h"

void testResizeBuffer1(CSimpleBuffer *m_buf, const char *t){
    m_buf->Write((void *)t, strlen(t));
}

void testResizeBuffer(const char* t){
    CSimpleBuffer m_buf = *new CSimpleBuffer();
    testResizeBuffer1(&m_buf,t);
    printf("buf:%s size:%d\n",m_buf.GetBuffer(),m_buf.GetWriteOffset());
}


TEST(pt_net, BufferResize) {
    char const* str = "name";
    testResizeBuffer(str);
    char const* str1 = "name1";
    testResizeBuffer(str1);
    ASSERT_TRUE(true);
}

TEST(pt_net, CSimpleBuffer) {
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

TEST(pt_net, socketSend) {
    CSimpleBuffer request = *new CSimpleBuffer();
    CSimpleBuffer response = *new CSimpleBuffer();
    string ip = "127.0.0.1";
    uint16_t port = 7841;
    char const* pduBytes = "name";
    request.Write((void *)pduBytes, strlen(pduBytes));
    socketSend(ip.c_str(),port,&request,&response);
    ASSERT_TRUE(true);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
