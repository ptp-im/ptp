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

#define CONFIG_PATH             "conf/bd_server.conf"
#define LOG_PATH                "log/default.log"

bool init_server_config();
void replace_config(const string &str,const string &replace_str);
void remove_config_path();
void set_config_path(const string &path);
string get_config_path();

string &get_config_str();
void set_config_str(const string &config_conf);

#endif //FILE_CONFIG_H
