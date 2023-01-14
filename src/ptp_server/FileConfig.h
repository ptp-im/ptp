//
// Created by jack on 2023/1/11.
//

#ifndef FILE_CONFIG_H
#define FILE_CONFIG_H

#include<cstdio>
#include<iostream>

#include "ptp_global/Helpers.h"
#include "ptp_global/Logger.h"

using namespace std;

#define CONFIG_PATH             "../conf/bd_server.conf"
#define LOG_PATH                "../log/default.log"

static string CONFIG_CONF = "" \
"MSG_ListenIP=0.0.0.0\n" \
"MSG_ListenPort=7881\n" \
"MSG_ConcurrentDBConnCnt=2\n" \
"MSG_DBServerIP1=127.0.0.1\n" \
"MSG_DBServerIP2=127.0.0.1\n" \
"MSG_DBServerPort1=7811\n" \
"MSG_DBServerPort2=7811\n" \
"MSG_ENABLE_BUSINESS_ASYNC=0\n\n" \

"Business_ListenIP=0.0.0.0\n" \
"Business_ListenPort=7811\n" \
"Business_ThreadNum=16\n\n" \

"DBInstances=bd_master,bd_slave\n" \
"bd_master_dbname=\n" \
"bd_master_host=127.0.0.1\n" \
"bd_master_port=3306\n" \
"bd_master_password=\n" \
"bd_master_username=\n" \
"bd_master_maxconncnt=16\n\n" \
"bd_slave_dbname=\n" \
"bd_slave_port=3306\n" \
"bd_slave_host=127.0.0.1\n" \
"bd_slave_password=\n" \
"bd_slave_username=\n" \
"bd_slave_maxconncnt=16\n\n" \

"CacheInstances=auth,group\n" \
"auth_host=127.0.0.1\n" \
"auth_port=6379\n" \
"auth_auth=\n" \
"auth_db=1\n" \
"auth_maxconncnt=16\n\n" \

"group_auth=s9mE_s3cUrE_prsS\n" \
"group_host=127.0.0.1\n" \
"group_port=6379\n" \
"group_db=1\n" \
"group_maxconncnt=16\n\n" \

"MSFS_BaseDir=../data/msfs\n" \
"MSFS_FileCnt=16\n" \
"MSFS_FilesPerDir=30000\n" \
"MSFS_GetThreadCount=32\n" \
"MSFS_ListenIP=0.0.0.0\n" \
"MSFS_ListenPort=7841\n" \
"MSFS_PostThreadCount=1\n" \
"MSFS_Site=127.0.0.1\n" \
"MsfsSite=127.0.0.1\n\n";

bool init_server_config();
void replace_config(string &config,const string &str,const string &replace_str);

#endif //FILE_CONFIG_H
