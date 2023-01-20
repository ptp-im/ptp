#include <gtest/gtest.h>

#include "ptp_global/Logger.h"
#include "ptp_global/Helpers.h"
#include "ptp_server/FileConfig.h"
#include "ptp_global/ConfigFileReader.h"

TEST(test_server, replace_str) {
    std::string base="this is a test2 test1 string.";
    replace_string(base,"test1","test");
    ASSERT_EQ(base,"this is a test2 test string.");
}

TEST(test_server, set_config) {
    set_config_path("conf/test.conf");
    remove_config_path();
    string config = get_config_str();
    replace_config("MSG_ListenIP=0.0.0.0","MSG_ListenIP=127.0.0.1");
    replace_config("MSG_ConcurrentDBConnCnt=2","MSG_ConcurrentDBConnCnt=20");
    string config1 = get_config_str();
    remove_config_path();
    init_server_config();
    CConfigFileReader config_file(get_config_path().c_str());
    char* msg_listen_ip = config_file.GetConfigName("MSG_ListenIP");
    string t(msg_listen_ip, strlen(msg_listen_ip));
    ASSERT_TRUE(t=="127.0.0.1");
    char* MSG_ConcurrentDBConnCnt = config_file.GetConfigName("MSG_ConcurrentDBConnCnt");
    string t1(MSG_ConcurrentDBConnCnt, strlen(MSG_ConcurrentDBConnCnt));
    ASSERT_TRUE(t1=="20");
}


TEST(test_server, init_server_config) {
    string config_path = "conf/test_config.conf";
    set_config_path(config_path);
    init_server_config();
    ASSERT_EQ(get_config_path(),config_path);
    ASSERT_EQ(file_exists(get_config_path().c_str()),true);
    remove_config_path();
    ASSERT_EQ(file_exists(get_config_path().c_str()),false);
}

TEST(test_server, config) {
    string CONFIG_CONF = get_config_str();
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