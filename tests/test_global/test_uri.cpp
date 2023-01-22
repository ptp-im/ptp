#include <gtest/gtest.h>
#include <array>
#include "ptp_global/SocketClient.h"
#include "ptp_global/Helpers.h"
#include "ptp_global/UriHelper.h"

using namespace std;

TEST(test_uri,Uri_parse) {
    string url1 = "https://www.google.com:8443/www/sss/?test=$%^&t1=222";
    Uri u1 = Uri::Parse(url1);
    ASSERT_EQ(u1.Protocol,"https");
    ASSERT_EQ(u1.Host,"www.google.com");
    ASSERT_EQ(u1.Port,8443);
    ASSERT_EQ(u1.Path,"/www/sss/");
    ASSERT_EQ(u1.QueryString,"?test=$%^&t1=222");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}