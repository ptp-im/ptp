#include <gtest/gtest.h>

#include "ptp_global/Logger.h"
#include "ptp_global/Helpers.h"
#include "ptp_server/FileConfig.h"


TEST(test_server, replace_str) {
    std::string base="this is a test string.";
    replace_string(base,"this","that");
    ASSERT_EQ(base,"that is a test string.");
}

TEST(test_server, init_server_config) {
    init_server_config();
    ASSERT_EQ(file_exists(CONFIG_PATH),true);
}

TEST(test_server, config) {
    DEBUG_I("CONFIG_PATH:%s",CONFIG_PATH);
    DEBUG_I("\n\n%s",CONFIG_CONF.c_str());
    string file_path = "/tmp/test.conf";
    if(file_exists(file_path.c_str())){
        DEBUG_I("file_path:%s exits",file_path.c_str());
        remove_file(file_path.c_str());
        ASSERT_EQ(file_exists(file_path.c_str()),false);
    }else{
        DEBUG_I("file_path:%s not exits",file_path.c_str());
        put_file_content(file_path.c_str(),(char *)CONFIG_CONF.c_str(),CONFIG_CONF.size());
        ASSERT_EQ(file_exists(file_path.c_str()),true);
        char content[CONFIG_CONF.size()];
        get_file_content(file_path.c_str(),content,CONFIG_CONF.size());
        DEBUG_I("%s",content);
        remove_file(file_path.c_str());
        ASSERT_EQ(file_exists(file_path.c_str()),false);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}