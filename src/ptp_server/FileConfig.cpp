#include "FileConfig.h"

bool init_server_config(){
    if(!file_exists(CONFIG_PATH)){
        auto dir = get_dir_path(CONFIG_PATH);
        if(!file_exists(dir.c_str())){
            make_dir(dir.c_str());
            if(!file_exists(dir.c_str())){
                DEBUG_E("Mkdir error:%s",dir.c_str());
                return false;
            }
        }
        DEBUG_I("Write config: %s",CONFIG_PATH);

        put_file_content(CONFIG_PATH,(char *)CONFIG_CONF.c_str(),CONFIG_CONF.size());
        if(!file_exists(CONFIG_PATH)){
            DEBUG_E("Write config error: %s",CONFIG_PATH);
            return false;
        }
    }else{
        DEBUG_I("Found config: %s",CONFIG_PATH);
    }
    return true;
}


void replace_config(string &config,const string &str,const string &replace_str){
    int index = (int)config.find_first_of(replace_str);
    config.replace(index,str.size(),replace_str);
}
