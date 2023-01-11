#include "InterLogin.h"
#include "CachePool.h"
#include "ptp_crypto//secp256k1_helpers.h"
#include "PTP.Auth.pb.h"

using namespace PTP::Common;

bool CInterLoginStrategy::ServerLogin(PTP::Server::ServerLoginReq *msg, PTP::Common::UserInfo *user, ERR &error){
    bool bRet = false;
    while (true) {
        DEBUG_D("captcha:%s",msg->captcha().c_str());
        DEBUG_D("address:%s",msg->address().c_str());
        DEBUG_D("sign:%s", bytes_to_hex_string(reinterpret_cast<const uint8_t *>(msg->sign().c_str()), 65).c_str());
        CacheManager *pCacheManager = CacheManager::getInstance();
        CacheConn *pCacheConn = pCacheManager->GetCacheConn("auth");
        if (!pCacheConn) {
            error = E_SYSTEM;
            DEBUG_E("error pCacheConn");
            break;
        }

        string msg_data = format_sign_msg_data(msg->captcha(),SignMsgType_ptp);

        unsigned char pub_key_33[33];
        string address_hex;

        bool ret = recover_pub_key_and_address_from_sig((unsigned char *)msg->sign().c_str(), msg_data, pub_key_33, address_hex);
        DEBUG_D("pub_key_33:%s", bytes_to_hex_string(pub_key_33, 33).c_str());
        DEBUG_D("address_hex rec:%s", address_hex.c_str());

        if(!ret){
            error = E_LOGIN_ERROR;
            DEBUG_E("sign rec error");
            break;
        }
        if(address_hex!=msg->address()){
            error = E_LOGIN_ERROR;
            DEBUG_E("client address:%s != %s",address_hex.c_str(), msg->address().c_str());
            break;
        }

        string pub_key_33_hex = bytes_to_hex_string(pub_key_33,33);
        uint32_t user_id;
        auto user_id_str= pCacheConn->get(CACHE_USER_ADR_ID_PREFIX + address_hex);
        if(user_id_str.empty()){
            auto user_id_long = pCacheConn->incr(CACHE_INCR_USER_ID);
            if(user_id_long == 1){
                user_id_long = pCacheConn->incrBy(CACHE_INCR_USER_ID,99999);
            }
            list<string> argv;
            string user_id_str1 = int2string(user_id_long);
            argv.emplace_back("MSET");
            argv.push_back(CACHE_USER_ADR_ID_PREFIX + address_hex);
            argv.push_back(user_id_str1);
            argv.push_back(CACHE_USER_ID_PUB_KEY_PREFIX + user_id_str1);
            argv.push_back(pub_key_33_hex);
            argv.push_back(CACHE_USER_ID_ADR_PREFIX + user_id_str1);
            argv.push_back(address_hex);
            argv.push_back(CACHE_USER_ID_LOGIN_TIME_PREFIX + user_id_str1);
            argv.push_back(int2string(unix_timestamp()));
            pCacheConn->exec(&argv, nullptr);
            user_id = (uint32_t)(atoi(user_id_str1.c_str()));
        }else{
            user_id = (uint32_t)(atoi(user_id_str.c_str()));
        }
        DEBUG_D("client address:%s, user_id:%d",address_hex.c_str(), user_id);
        user->set_uid(user_id);
        user->set_address(address_hex);
        user->set_pub_key(pub_key_33,33);

        list<string> argv;
        list<string> list_ret;
        string user_id_str1 = int2string(user_id);
        argv.emplace_back("MGET");
        argv.push_back(CACHE_USER_ID_AVATAR_PREFIX      + user_id_str1);
        argv.push_back(CACHE_USER_ID_NICKNAME_PREFIX    + user_id_str1);
        argv.push_back(CACHE_USER_ID_USERNAME_PREFIX    + user_id_str1);
        argv.push_back(CACHE_USER_ID_SIGN_INFO_PREFIX   + user_id_str1);
        argv.push_back(CACHE_USER_ID_FIRST_NAME_PREFIX   + user_id_str1);
        argv.push_back(CACHE_USER_ID_LAST_NAME_PREFIX   + user_id_str1);
        argv.push_back(CACHE_USER_ID_LOGIN_TIME_PREFIX   + user_id_str1);
        argv.push_back(CACHE_USER_ID_STATUS_PREFIX      + user_id_str1);

        pCacheConn->exec(&argv, &list_ret);

        auto it = list_ret.begin();
        user->set_avatar(it->c_str());
        advance(it,1);
        user->set_nick_name(it->c_str());
        advance(it,1);
        user->set_user_name(it->c_str());
        advance(it,1);
        user->set_sign_info(it->c_str());
        advance(it,1);
        user->set_first_name(it->c_str());
        advance(it,1);
        user->set_last_name(it->c_str());
        advance(it,1);
        user->set_login_time(string2int(it->c_str()));
        advance(it,1);
        user->set_status(it->empty() ? 0 : string2int(it->c_str()));
        pCacheManager->RelCacheConn(pCacheConn);
        break;
    }
    return bRet;
}
