//
// Created by jack on 2023/1/15.
//

#ifndef PTP_CTL_TEST_DEFINE_H
#define PTP_CTL_TEST_DEFINE_H

#include "ptp_server/MsgSrvConn.h"
#include "ptp_server/CachePool.h"
#include "ptp_server/FileConfig.h"

#define TEST_CONFIG_PATH "conf/test.conf"

#define TEST_CONFIG_CacheInstances  "CacheInstances=auth,group"
#define TEST_CONFIG_auth_host       "auth_host=127.0.0.1"
#define TEST_CONFIG_auth_port       "auth_port=6379"
#define TEST_CONFIG_auth_auth       "auth_auth=s9mE_s3cUrE_prsS"
#define TEST_CONFIG_auth_db         "auth_db=11"
#define TEST_CONFIG_auth_maxconncnt "auth_maxconncnt=16"

#define TEST_CONFIG_group_host       "group_host=127.0.0.1"
#define TEST_CONFIG_group_port       "group_port=6379"
#define TEST_CONFIG_group_auth       "group_auth=s9mE_s3cUrE_prsS"
#define TEST_CONFIG_group_db         "group_db=12"
#define TEST_CONFIG_group_maxconncnt "group_maxconncnt=16"

#define DEFAULT_TEST_ACCOUNT_ID 10011

void test_init();
uint32_t get_test_default_group_id();
CMsgSrvConn *test_init_msg_conn();
void login(CMsgSrvConn *pMsgConn,uint32_t accountId);
void createGroup(CMsgSrvConn *pMsgConn, PTP::Common::GroupType groupType, uint32_t fromUid, uint32_t toUid, uint32_t accountId = 1001);

#endif //PTP_CTL_TEST_DEFINE_H
