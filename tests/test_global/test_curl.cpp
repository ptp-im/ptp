#include <gtest/gtest.h>
#include <array>
#include "ptp_global/Helpers.h"
#include "ptp_global/Logger.h"
#include "ptp_curl/CurlHelper.h"

using namespace std;

TEST(test_curl, Get) {
    string strUrl = "https://www.google.com";
    string response;
    auto res = curl_get(strUrl,response);
    DEBUG_D("%s",response.c_str());
    ASSERT_EQ(res,CURLE_OK);
    ASSERT_TRUE(!response.empty());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}