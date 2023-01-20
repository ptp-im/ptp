#include "InterLogin.h"
#include "CachePool.h"
#include "ptp_crypto/secp256k1_helpers.h"
#include "PTP.Auth.pb.h"
#include "actions/models/ModelBuddy.h"

using namespace PTP::Common;

bool CInterLoginStrategy::ServerLogin(PTP::Server::ServerLoginReq *msg, PTP::Common::UserInfo *user, ERR &error){
    bool bRet = false;
    while (true) {
        DEBUG_D("captcha:%s",msg->captcha().c_str());
        DEBUG_D("client address:%s",msg->address().c_str());
        DEBUG_D("client sign:%s", bytes_to_hex_string((unsigned char *)msg->sign().data(), 65).c_str());
        CacheManager *pCacheManager = CacheManager::getInstance();
        CacheConn *pCacheConn = pCacheManager->GetCacheConn("auth");
        if (!pCacheConn) {
            error = E_SYSTEM;
            DEBUG_E("error pCacheConn");
            break;
        }

        string msg_data = format_sign_msg_data(msg->captcha(),SignMsgType_ptp);
        DEBUG_D("sign msg_data:%s", msg_data.c_str());

        unsigned char pub_key_33[33];
        string address_hex;

        bool ret = recover_pub_key_and_address_from_sig((unsigned char *)msg->sign().c_str(), msg_data, pub_key_33, address_hex);
        DEBUG_D("client pub_key_33 rec:%s", bytes_to_hex_string(pub_key_33, 33).c_str());
        DEBUG_D("client address rec:%s", address_hex.c_str());

        if(!ret){
            error = E_LOGIN_ERROR;
            DEBUG_E("sign rec error");
            break;
        }
        if(address_hex!= msg->address()){
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
        CModelBuddy::getUserInfo(pCacheConn,user,user_id);
        pCacheManager->RelCacheConn(pCacheConn);
        break;
    }
    return bRet;
}
