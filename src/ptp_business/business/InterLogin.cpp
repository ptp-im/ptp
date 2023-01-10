/*================================================================
*     Copyright (c) 2015年 lanhu. All rights reserved.
*   
*   文件名称：InterLogin.cpp
*   创 建 者：PTP
*   邮    箱：crypto.service@proton.me
*   创建日期：2015年03月09日
*   描    述：
*
================================================================*/
#include "InterLogin.h"
#include "DBPool.h"
#include "CachePool.h"
#include "ptp_crypto/secp256k1_helpers.h"
#include <cryptopp/osrng.h>
#include "helpers.h"
#include "PTP.Auth.pb.h"

using namespace PTP::Common;


bool CInterLoginStrategy::ServerLogin(const string sign, const string &captcha, const string &address, PTP::Common::UserInfo *user, ERR &error){
    bool bRet = false;
    while (1) {
        log_debug("captcha:%s",captcha.c_str());
        CacheManager *pCacheManager = CacheManager::getInstance();
        CacheConn *pCacheConn = pCacheManager->GetCacheConn("auth");
        if (!pCacheConn) {
            error = E_SYSTEM;
            log_error("error pCacheConn");
            break;
        }

        string msg_data = format_sign_msg_data(captcha,SignMsgType_ptp);

        unsigned char pub_key_33[33];
        string address_hex;
        auto ret = recover_pub_key_from_sig_65((unsigned char *)sign.data(),msg_data, pub_key_33,address_hex);

        if(!ret){
            error = E_LOGIN_ERROR;
            log_error("sign rec error");
            break;
        }
        if(address_hex!=address){
            error = E_LOGIN_ERROR;
            log_error("address_hex:%s != %s",address_hex.c_str(), address.c_str());
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
        log_debug("address_hex:%s, user_id:%d",address_hex.c_str(), user_id);
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
        if (pCacheConn) {
            pCacheManager->RelCacheConn(pCacheConn);
        }
        break;
    }
    return bRet;
}

bool CInterLoginStrategy::doLogin(const std::string &strName, const std::string &strPass, IM::BaseDefine::UserInfo& user)
{
    bool bRet = false;
    CDBManager* pDBManger = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManger->GetDBConn("bd_slave");
    if (pDBConn) {
        string strSql = "select * from bd_im_user where user_name='" + strPass + "'";
        CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
        if(pResultSet)
        {
            string strResult, strSalt;
            uint32_t nId, nGender, nDeptId, nStatus;
            string strNick, strAvatar, strEmail, strUserName, strTel, strDomain,strSignInfo;
            bool is_res = false;
            while (pResultSet->Next()) {
                nId = pResultSet->GetInt("id");
                strNick = pResultSet->GetString("nick_name");
                nGender = pResultSet->GetInt("sex");
                strUserName = pResultSet->GetString("user_name");
                strDomain = pResultSet->GetString("domain");
                strTel = pResultSet->GetString("phone");
                strEmail = pResultSet->GetString("email");
                strAvatar = pResultSet->GetString("avatar");
                nDeptId = pResultSet->GetInt("departId");
                nStatus = pResultSet->GetInt("status");
                strSignInfo = pResultSet->GetString("sign_info");
                is_res = true;
            }

            if(is_res){
                bRet = true;
                user.set_user_id(nId);
                user.set_user_nick_name(strNick);
                user.set_user_gender(nGender);
                user.set_user_real_name(strUserName);
                user.set_user_domain(strDomain);
                user.set_user_tel(strTel);
                user.set_email(strEmail);
                user.set_avatar_url(strAvatar);
                user.set_department_id(nDeptId);
                user.set_status(nStatus);
                user.set_sign_info(strSignInfo);
            }else{
                CDBConn* pDBConn = pDBManger->GetDBConn("bd_master");
                if (pDBConn)
                {
                    int created = unix_timestamp();
                    int updated = created;
                    strDomain = "wwww";
                    string strSql = "insert into bd_im_user(`user_name`,`password`,`domain`, `salt`,`created`,`updated`) values(?, ?, ?, ?, ?, ?)";

                    CPrepareStatement* stmt = new CPrepareStatement();
                    if (stmt->Init(pDBConn->GetMysql(), strSql))
                    {
                        uint32_t index = 0;
                        stmt->SetParam(index++, strName);
                        stmt->SetParam(index++, strPass);
                        stmt->SetParam(index++, strDomain);
                        stmt->SetParam(index++, "");
                        stmt->SetParam(index++, created);
                        stmt->SetParam(index++, updated);
                        bRet = stmt->ExecuteUpdate();
                        if (!bRet)
                        {
                            log("insert user failed: %s", strSql.c_str());
                        }else{
                            nId = pDBConn->GetInsertId();
                            user.set_user_id(nId);
                            user.set_user_nick_name("");
                            user.set_user_gender(0);
                            user.set_user_real_name(strName);
                            user.set_user_domain(strDomain);
                            user.set_user_tel("");
                            user.set_email("");
                            user.set_avatar_url("");
                            user.set_department_id(0);
                            user.set_status(0);
                            user.set_sign_info("");
                        };

                    }
                    delete stmt;
                }
                else
                {
                    log_error("no db connection for bd_master");
                }
            }
            delete  pResultSet;
        }
        pDBManger->RelDBConn(pDBConn);
    }
    return bRet;
}
