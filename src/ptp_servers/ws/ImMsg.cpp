
#include "CachePool.h"
#include "ImMsg.h"
#include "ImUser.h"

CImMsg::CImMsg() {

}


CImMsg::~CImMsg() {

}

uint32_t GetMsgIdFromCache(string from_address,string to_address)
{
    auto from_uid = GetUserIdFromCache(from_address,false);
    auto to_uid = GetUserIdFromCache(to_address,false);
    if(from_uid == 0 || to_uid == 0){
        return 0;
    }
    auto from_uid_str = to_string(from_uid);
    auto to_uid_str = to_string(to_uid);
    string rel_key = from_uid > to_uid ? to_uid_str + "_" + from_uid_str : from_uid_str + "_" + to_uid_str;

    CacheManager* pCacheManager = CacheManager::getInstance();
    CacheConn* pCacheConn = pCacheManager->GetCacheConn("msg");
    if (pCacheConn) {
        auto msg_id_long = pCacheConn->incr("msg_id_" + rel_key);
        pCacheManager->RelCacheConn(pCacheConn);
        return (uint32_t)msg_id_long;
    }else{
        DEBUG_E("error pCacheConn");
        return 0;
    }
}