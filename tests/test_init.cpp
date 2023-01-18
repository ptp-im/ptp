//
// Created by jack on 2023/1/15.
//
#include "test_init.h"

void test_int(){
    set_config_path(TEST_CONFIG_PATH);
    remove_config_path();

    replace_config("CacheInstances=auth,group",TEST_CONFIG_CacheInstances);
    replace_config("auth_host=127.0.0.1",TEST_CONFIG_auth_host);
    replace_config("auth_port=6379",TEST_CONFIG_auth_port);
    replace_config("auth_auth=",TEST_CONFIG_auth_auth);
    replace_config("auth_db=1",TEST_CONFIG_auth_db);
    replace_config("auth_maxconncnt=16",TEST_CONFIG_auth_maxconncnt);

    replace_config("group_host=127.0.0.1",TEST_CONFIG_group_host);
    replace_config("group_port=6379",TEST_CONFIG_group_port);
    replace_config("group_auth=",TEST_CONFIG_group_auth);
    replace_config("group_db=127.0.0.1",TEST_CONFIG_group_db);
    replace_config("group_maxconncnt=16",TEST_CONFIG_group_maxconncnt);

    remove_config_path();
    init_server_config();
    CacheManager::setConfigPath(get_config_path());
    CacheManager::getInstance();
}