#include "FileConfig.h"

static string m_config_path = "conf/bd_server.conf";
static string m_config_conf_temp = "" \
"WS_ListenIP=0.0.0.0\n" \
"WS_ListenPort=7871\n" \
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
"group_host=127.0.0.1\n" \
"group_port=6379\n" \
"group_auth=\n" \
"group_db=2\n" \
"group_maxconncnt=16\n\n" \
"PUSH_ListenIP=0.0.0.0\n" \
"PUSH_ListenPort=7861\n" \
"CertPath=conf/apns-dev-cert.pem\n" \
"KeyPath=conf/apns-dev-key.pem\n" \
"KeyPassword=ZZZ@WWWW\n" \
"SandBox=1\n\n" \
"LOGIN_ClientListenIP=0.0.0.0\n" \
"LOGIN_ClientPort=7831\n" \
"LOGIN_HttpListenIP=0.0.0.0\n" \
"LOGIN_HttpPort=7831\n" \
"LOGIN_MSG_ListenIP=0.0.0.0\n" \
"LOGIN_MSG_ListenPort=7833\n" \
"URL_msfs=http://127.0.0.1:7841\n" \
"URL_discovery=http://127.0.0.1\n\n" \
"ROUTE_ListenIP=0.0.0.0\n" \
"ROUTE_ListenPort=7851\n\n" \
"MSFS_BaseDir=./data/msfs\n" \
"MSFS_FilesPerDir=30000\n" \
"MSFS_GetThreadCount=32\n" \
"MSFS_PostThreadCount=1\n" \
"MSFS_ListenIP=0.0.0.0\n" \
"MSFS_ListenPort=7841\n" \
"MSFS_Site=127.0.0.1\n" \
"MsfsSite=127.0.0.1\n\n";

static string m_config_conf;

void set_config_path(const string &path){
    m_config_path = path;
}

string get_config_path(){
    return m_config_path;
}

void remove_config_path(){
    remove_file(m_config_path.c_str());
}

bool init_server_config(){
    if(!file_exists(m_config_path.c_str())){
        auto dir = get_dir_path(m_config_path);
        if(!file_exists(dir.c_str())){
            make_dir(dir.c_str());
            if(!file_exists(dir.c_str())){
                DEBUG_E("Mkdir error:%s",dir.c_str());
                return false;
            }
        }
        DEBUG_I("Write config: %s",m_config_path.c_str());
        string config_conf = get_config_str();
        put_file_content(m_config_path.c_str(),(char *)config_conf.c_str(),config_conf.size());
        if(!file_exists(m_config_path.c_str())){
            DEBUG_E("Write config error: %s",m_config_path.c_str());
            return false;
        }
    }else{
        DEBUG_I("Found config: %s",m_config_path.c_str());
    }
    return true;
}

void replace_config(const string &str,const string &replace_str){
    string config = get_config_str();
    auto index = config.find(str,0);
    if(index != string::npos){
        config.replace(index,str.size(),replace_str);
        set_config_str(config);
    }
}

string &get_config_str(){
    if(m_config_conf.empty()){
        set_config_str(m_config_conf_temp);
    }
    return m_config_conf;
}

void set_config_str(const string &config_conf){
    m_config_conf = config_conf;
}
