/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelGroup.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Saturday, November 5, 2022
 *   desc：
 *
 *
================================================================*/
#include "ModelGroup.h"
#include "CachePool.h"
#include "PTP.Common.pb.h"
#include "helpers.h"
#include "secp256k1_helpers.h"

CModelGroup* CModelGroup::m_pInstance = NULL;

CModelGroup::CModelGroup()
{
    
}

CModelGroup::~CModelGroup()
{
    
}
CModelGroup* CModelGroup::getInstance()
{
    if (m_pInstance == NULL) {
        m_pInstance = new CModelGroup();
    }
    return m_pInstance;
}

void CModelGroup::GroupPreCreateReq(PTP::Group::GroupPreCreateReq* msg, PTP::Group::GroupPreCreateRes* msg_rsp, PTP::Common::ERR& error)
{
    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn("group");

    string groupAddress;
    uint32_t groupIndex = 0;

    while (true) {
        if (!pCacheConn) {
            error = PTP::Common:: E_SYSTEM;
            log_error("error pCacheConn");
            break;
        }

        const auto& members = msg->members();
        auto groupType = msg->group_type();
        auto fromUserId = msg->auth_uid();
        auto fromUserId_str = to_string(fromUserId);

        if (groupType == PTP::Common::GROUP_TYPE_MULTI) {
            auto group_index_long = pCacheConn->incr(GROUP_IDX_BY_UID_INCR_PREFIX + fromUserId_str);
            groupIndex = (u_int32_t) group_index_long;
        }

        if (groupType == PTP::Common::GROUP_TYPE_PAIR) {
            uint32_t toUserId;
            string relId;
            if (members.empty()) {
                toUserId = fromUserId;
            } else {
                toUserId = members.Get(0);
            }

            if (toUserId == 0) {
                error = PTP::Common::E_GROUP_CREATE_PAIR_GROUP_NO_REG_USER;
                break;
            }
            string group_id_str;
            if (toUserId >= fromUserId) {
                group_id_str = pCacheConn->hget(GROUP_PAIR_USER_REL_PREFIX + to_string(fromUserId), to_string(toUserId));
            } else {
                group_id_str = pCacheConn->hget(GROUP_PAIR_USER_REL_PREFIX + to_string(toUserId), to_string(fromUserId));
            }

            if (group_id_str.empty()) {
                auto group_index_long = pCacheConn->incr(GROUP_IDX_BY_UID_INCR_PREFIX + to_string(fromUserId));
                groupIndex = (u_int32_t) group_index_long;
                break;
            } else {
                groupAddress = pCacheConn->get(GROUP_ADDRESS_PREFIX + group_id_str);
                string group_idx_str = pCacheConn->get(GROUP_IDX_PREFIX + group_id_str);
                groupIndex = string2int(group_idx_str);
            }
        }
        break;
    }

    msg_rsp->set_group_adr(groupAddress);
    msg_rsp->set_group_idx(groupIndex);
    if (pCacheConn) {
        pCacheManager->RelCacheConn(pCacheConn);
    }
}

void CModelGroup::GroupCreateReq(PTP::Group::GroupCreateReq* msg, PTP::Group::GroupCreateRes* msg_rsp, PTP::Common::ERR& error)
{
    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn("group");

    while (true) {
        if (!pCacheConn) {
            error = PTP::Common:: E_SYSTEM;
            log_error("error pCacheConn");
            break;
        }

        string sign = msg->sign();
        string captcha = msg->captcha();
        uint32_t groupIndex = msg->group_idx();
        string groupName = msg->name();
        string groupAvatar = msg->avatar();
        string about = msg->about();
        auto groupType = msg->group_type();
        auto members = msg->members();
        auto pub_key_64 = recover_pub_key_from_sig(sign, format_sign_msg_data(to_string(groupIndex) + captcha,SignMsgType_ptp_group));
        if(pub_key_64.empty()){
            error = PTP::Common:: E_SYSTEM;
            log_error("error pub_key_64 is null");
            break;
        }
        auto groupAddress_str = pub_key_to_address(pub_key_64);
        auto groupAddress = address_to_hex(groupAddress_str);
        log("groupAddress: %s",groupAddress.c_str());

        auto fromUserId = msg->auth_uid();
        string fromUserId_str = to_string(fromUserId);
        string group_id_str = pCacheConn->get(GROUP_ADDRESS_ID_PREFIX + groupAddress);

        list<string> group_members_id;
        if (group_id_str.empty()) {
            uint32_t created_time = time(nullptr);
            PTP::Common::GroupInfo *group = msg_rsp->mutable_group();
            group->set_group_id(string2int(group_id_str));
            if (groupType == PTP::Common::GROUP_TYPE_PAIR) {
                auto toUserId = members.Get(0);
                if (toUserId >= fromUserId) {
                    string group_id_str = pCacheConn->hget(GROUP_PAIR_USER_REL_PREFIX + to_string(fromUserId), to_string(toUserId));
                    if(!group_id_str.empty()){
                        error = PTP::Common::E_SYSTEM;
                        log_error("GROUP_TYPE_PAIR exists");
                        break;
                    }
                    pCacheConn->hset(GROUP_PAIR_USER_REL_PREFIX + to_string(fromUserId), to_string(toUserId),
                                     group_id_str);

                } else {
                    string group_id_str = pCacheConn->hget(GROUP_PAIR_USER_REL_PREFIX + to_string(toUserId), to_string(fromUserId));
                    if(!group_id_str.empty()){
                        error = PTP::Common::E_SYSTEM;
                        log_error("GROUP_TYPE_PAIR exists");
                        break;
                    }
                    pCacheConn->hset(GROUP_PAIR_USER_REL_PREFIX + to_string(toUserId), to_string(fromUserId),
                                     group_id_str);
                }
                list<string> argv_member_pair_update;
                argv_member_pair_update.emplace_back("ZADD");
                argv_member_pair_update.push_back(MEMBER_PAIR_UPDATE_PREFIX+fromUserId_str);
                argv_member_pair_update.push_back(to_string(created_time));
                argv_member_pair_update.push_back(to_string(toUserId));
                pCacheConn->exec(&argv_member_pair_update, nullptr);

                list<string> argv_member_pair_update1;
                argv_member_pair_update1.emplace_back("ZADD");
                argv_member_pair_update1.push_back(MEMBER_PAIR_UPDATE_PREFIX+to_string(toUserId));
                argv_member_pair_update1.push_back(to_string(created_time));
                argv_member_pair_update1.push_back(fromUserId_str);
                pCacheConn->exec(&argv_member_pair_update1, nullptr);
                group->set_pair_uid(toUserId);
            }
            list<string> group_member_ids;
            auto group_id_long = pCacheConn->incr(GROUP_ID_INCR_PREFIX);
            if(group_id_long == 1){
                group_id_long =  pCacheConn->incrBy(GROUP_ID_INCR_PREFIX,599);
            }
            uint32_t group_id = (uint32_t) group_id_long;
            group_id_str = to_string(group_id);
            string groupIndex_str =  to_string(groupIndex);
            string groupType_str = to_string(groupType);

            list<string> set_argv;

            set_argv.emplace_back("MSET");
            set_argv.push_back(GROUP_ADDRESS_ID_PREFIX + groupAddress);
            set_argv.push_back(group_id_str);
            set_argv.push_back(GROUP_IDX_PREFIX + group_id_str);
            set_argv.push_back(groupIndex_str);

            set_argv.push_back(GROUP_TYPE_PREFIX + group_id_str);
            set_argv.push_back(groupType_str);

            set_argv.push_back(GROUP_ADDRESS_PREFIX + group_id_str);
            set_argv.push_back(groupAddress);

            set_argv.push_back(GROUP_UPDATE_PREFIX + group_id_str);
            set_argv.push_back(to_string(created_time));

            pCacheConn->exec(&set_argv, nullptr);

            group_member_ids.push_back(fromUserId_str);

            PTP::Common::GroupMember* groupMember = msg_rsp->add_group_members();
            groupMember->set_uid(msg->auth_uid());
            groupMember->set_member_status(PTP::Common::GROUP_MEMBER_STATUS_NORMAL);

            for (auto it = members.begin(); it != members.end(); it++) {
                uint32_t member_uid = *it;
                if (member_uid > 0) {
                    PTP::Common::GroupMember* groupMember1 = msg_rsp->add_group_members();
                    group_member_ids.push_back(to_string(member_uid));
                    groupMember1->set_uid(member_uid);
                    groupMember1->set_member_status(PTP::Common::GROUP_MEMBER_STATUS_NORMAL);
                }
            }

            list<string> group_member_status_argv;
            list<string> group_member_update_argv;

            for (auto it = group_member_ids.begin(); it != group_member_ids.end(); it++) {
                string member_id = it->c_str();
                group_member_status_argv.push_back(to_string(PTP::Common::GROUP_MEMBER_STATUS_NORMAL));
                group_member_status_argv.push_back(member_id);

                group_member_update_argv.push_back(to_string(created_time));
                group_member_update_argv.push_back(member_id);

                list<string> member_group_argv;
                member_group_argv.push_back(group_id_str);

                pCacheConn->cmd(
                        "SADD",
                        MEMBER_GROUP_KEY_PREFIX + member_id,
                        &member_group_argv,
                        nullptr);
            }

            pCacheConn->cmd(
                    "ZADD",
                    GROUP_MEMBER_STATUS_KEY_PREFIX + group_id_str,
                    &group_member_status_argv,
                    nullptr);

            pCacheConn->cmd(
                    "ZADD",
                    GROUP_MEMBER_UPDATE_KEY_PREFIX + group_id_str,
                    &group_member_update_argv,
                    nullptr);

            if (groupType == PTP::Common::GROUP_TYPE_PAIR) {
                auto toUserId = members.Get(0);
                if (toUserId >= fromUserId) {
                    pCacheConn->hset(GROUP_PAIR_USER_REL_PREFIX + to_string(fromUserId), to_string(toUserId),
                                     group_id_str);
                } else {
                    pCacheConn->hset(GROUP_PAIR_USER_REL_PREFIX + to_string(toUserId), to_string(fromUserId),
                                     group_id_str);
                }
                group->set_pair_uid(toUserId);
            }

            group->set_group_id(group_id_long);
            group->set_group_adr(groupAddress);
            group->set_group_idx(groupIndex);
            group->set_name(groupName);
            group->set_avatar(groupAvatar);
            group->set_created_time(created_time);
            group->set_owner_uid(msg->auth_uid());
            group->set_group_type(groupType);
            string group_seri = group->SerializeAsString();
            string group_info_string = bytes_to_hex_string(reinterpret_cast<const uint8_t *>(group_seri.data()), group_seri.size());
            pCacheConn->set(GROUP_INFO_PREFIX + group_id_str, group_info_string);
        } else {
            error = PTP::Common::E_GROUP_HAS_CREATED;
        }
        break;
    }

    if (pCacheConn) {
        pCacheManager->RelCacheConn(pCacheConn);
    }
}

void CModelGroup::GroupGetListReq(PTP::Group::GroupGetListReq* msg, PTP::Group::GroupGetListRes* msg_rsp, PTP::Common::ERR& error){
    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn("group");
    while (true) {
        if (!pCacheConn) {
            error = PTP::Common:: E_SYSTEM;
            log_error("error pCacheConn");
            break;
        }
        list<string> ret_list;
        list<string> argv;
        argv.emplace_back("SMEMBERS");
        argv.push_back(MEMBER_GROUP_KEY_PREFIX + to_string(msg->auth_uid()));
        pCacheConn->exec(&argv,&ret_list);

        list<string> ret_list1;
        list<string> argv1;
        argv1.emplace_back("MGET");
        for (auto it = ret_list.begin(); it != ret_list.end(); it++) {
            string group_id_str = it->c_str();
            argv1.push_back(GROUP_UPDATE_PREFIX + group_id_str);
        }

        list<string> argv_list_group_m_latest_msg;
        list<string> ret_list_group_m_latest_msg;
        argv_list_group_m_latest_msg.emplace_back("MGET");
        pCacheConn->exec(&argv1,&ret_list1);
        log("groups size: %d",ret_list.size());

        auto it_group = ret_list.begin();
        uint32_t updated_time = msg->group_info_updated_time();
        for (auto it = ret_list1.begin(); it != ret_list1.end(); it++) {
            auto group_update_time = it->c_str();
            if((uint32_t)atoi(group_update_time) > updated_time){
                log(" %d %d  %s",atoi(group_update_time),updated_time,it_group->c_str());
                argv_list_group_m_latest_msg.push_back(GROUP_INFO_PREFIX + string(it_group->c_str()));
            }
            it_group++;
        }
        pCacheConn->exec(&argv_list_group_m_latest_msg,&ret_list_group_m_latest_msg);

        for (auto it = ret_list_group_m_latest_msg.begin(); it != ret_list_group_m_latest_msg.end(); it++) {
            string group_info = it->c_str();
            PTP::Common::GroupInfo * group = msg_rsp->add_groups();
            string group_info_string = hex_to_string(group_info.substr(2));
            group->ParseFromString(group_info_string);
        }

        break;
    }

    if (pCacheConn) {
        pCacheManager->RelCacheConn(pCacheConn);
    }
}
void CModelGroup::GroupGetMembersListReq(PTP::Group::GroupGetMembersListReq* msg, PTP::Group::GroupGetMembersListRes* msg_rsp, PTP::Common::ERR& error){
    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn("group");
    while (true) {
        if (!pCacheConn) {
            error = PTP::Common:: E_SYSTEM;
            log_error("error pCacheConn");
            break;
        }

        auto group_members_updated_time = msg->group_members_updated_time();
        auto group_id = msg->group_id();

        auto userId = msg->auth_uid();

        list<string> argv0;
        list<string> ret_list0;

        argv0.emplace_back("ZRANGE");
        argv0.push_back(GROUP_MEMBER_STATUS_KEY_PREFIX + to_string(group_id));
        argv0.push_back("0");
        argv0.emplace_back("+INF");
        argv0.emplace_back("BYSCORE");
        argv0.emplace_back("WITHSCORES");

        pCacheConn->exec(&argv0, &ret_list0);

        if(ret_list0.size() > 0){
            for(auto it = ret_list0.begin();it != ret_list0.end();it++){
                advance(it,0);
                uint32_t user_id_str = string2int(it->c_str());
                advance(it,1);
                PTP::Common::GroupMemberStatus status = (PTP::Common::GroupMemberStatus)string2int(it->c_str());
                PTP::Common::GroupMember* group_member = msg_rsp->add_group_members();
                group_member->set_uid(user_id_str);
                group_member->set_member_status(status);
            }
        }
        list<string> argv;
        list<string> ret_list;
        argv.emplace_back("ZRANGE");
        argv.push_back(GROUP_MEMBER_UPDATE_KEY_PREFIX + to_string(group_id));
        argv.push_back(to_string(group_members_updated_time));
        argv.emplace_back("+INF");
        argv.emplace_back("BYSCORE");
        pCacheConn->exec(&argv,&ret_list);
        list<string> argv_users;
        list<string> list_users;
        argv_users.emplace_back("MGET");

        for(auto it = ret_list.begin();it != ret_list.end();it++){
            string pair_uid = it->c_str();
            argv_users.push_back(CACHE_USER_ID_ADR_PREFIX + pair_uid);
            argv_users.push_back(CACHE_USER_ID_AVATAR_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_NICKNAME_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_USERNAME_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_PUB_KEY_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_SIGN_INFO_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_FIRST_NAME_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_LAST_NAME_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_LOGIN_TIME_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_STATUS_PREFIX+ pair_uid);
        }
        pCacheConn->exec(&argv_users,&list_users);
        auto it_pair = ret_list.begin();
        for (auto it = list_users.begin(); it != list_users.end() ; it++) {
            PTP::Common::UserInfo * member = msg_rsp->add_members();
            member->set_uid(string2int(it_pair->c_str()));
            member->set_address(it->c_str());
            advance(it,1);
            member->set_avatar(it->c_str());
            advance(it,1);
            member->set_nick_name(it->c_str());
            advance(it,1);
            member->set_user_name(it->c_str());
            advance(it,1);
            member->set_pub_key(it->c_str());
            advance(it,1);
            member->set_sign_info(it->c_str());
            advance(it,1);
            member->set_first_name(it->c_str());
            advance(it,1);
            member->set_last_name(it->c_str());
            advance(it,1);
            member->set_login_time(string2int(it->c_str()));
            advance(it,1);
            member->set_status(string2int(*it));
            it_pair++;

        }
        msg_rsp->set_group_id(group_id);
        msg_rsp->set_group_members_updated_time(time(nullptr));
        break;
    }

    if (pCacheConn) {
        pCacheManager->RelCacheConn(pCacheConn);
    }
}

void CModelGroup::getGroupMembers(uint32_t nGroupId, list<uint32_t>& lsUserId){
    list<string> members;
    list<string> argv;
    argv.emplace_back("0");
    argv.emplace_back("-1");
    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn("group");
    while (true) {
        if (!pCacheConn) {
            log_error("error pCacheConn");
            break;
        }

        pCacheConn->cmd("ZRANGE", GROUP_MEMBER_STATUS_KEY_PREFIX + to_string(nGroupId), &argv, &members);
        if (pCacheConn) {
            pCacheManager->RelCacheConn(pCacheConn);
        }
        if (members.size() > 0) {
            for (auto it = members.begin(); it != members.end(); it++) {
                lsUserId.push_back(string2int(it->c_str()));
            }
        }
        break;
    }

}

void CModelGroup::GroupModifyReq(PTP::Group::GroupModifyReq* msg, PTP::Group::GroupModifyRes* msg_rsp, PTP::Common::ERR& error){
    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn("group");
    while (true) {
        if (!pCacheConn) {
            error = PTP::Common:: E_SYSTEM;
            log_error("error pCacheConn");
            break;
        }
        string value = msg->value();
        auto group_modify_action = msg->group_modify_action();
        uint32_t group_id = msg->group_id();
        string group_id_str = to_string(group_id);
        string groupType_str = pCacheConn->get(GROUP_TYPE_PREFIX + group_id_str);
        auto groupType = (PTP::Common::GroupType) string2int(groupType_str);
        if(groupType == PTP::Common::GROUP_TYPE_PAIR){
            error = PTP::Common::E_SYSTEM;
            log_error("pair group can not modify");
            break;
        }

        string group_info_hex = pCacheConn->get(GROUP_INFO_PREFIX + group_id_str);
        string group_info_buf = hex_to_string(group_info_hex.substr(2));
        PTP::Common::GroupInfo group;
        group.ParseFromArray(group_info_buf.data(), group_info_buf.length());

        auto fromUserId = msg->auth_uid();
        auto fromUserId_str = to_string(fromUserId);

        if(group.owner_uid() != fromUserId){
            error = PTP::Common::E_SYSTEM;
            log_error("you are not owner to modify group");
            break;
        }

        switch (group_modify_action) {
            case PTP::Common::GroupModifyAction_avatar:
                group.set_avatar(value);
                break;
            case PTP::Common::GroupModifyAction_name:
                group.set_name(value);
                break;
            default:
                break;
        }

        string group_ = group.SerializeAsString();
        string group_info_string = bytes_to_hex_string(reinterpret_cast<const uint8_t *>(group_.data()), group_.size());

        uint32_t updated_time = time(nullptr);

        list<string> argv_set;
        argv_set.emplace_back("MSET");
        argv_set.emplace_back(GROUP_INFO_PREFIX + group_id_str);
        argv_set.emplace_back(group_info_string);
        argv_set.emplace_back(GROUP_UPDATE_PREFIX + group_id_str);
        argv_set.emplace_back(to_string(updated_time));
        pCacheConn->exec(&argv_set, nullptr);
        msg_rsp->set_group_id(group_id);
        msg_rsp->set_group_modify_action(group_modify_action);
        list<uint32_t> lsUserId;
        getGroupMembers(msg->group_id(),lsUserId);
        if(!lsUserId.empty()){
            for (auto it = lsUserId.begin();it!= lsUserId.end();it++) {
                msg_rsp->add_notify_members(*it);
            }
        }
        break;
    }

    if (pCacheConn) {
        pCacheManager->RelCacheConn(pCacheConn);
    }
}
void CModelGroup::GroupUnreadMsgReq(PTP::Group::GroupUnreadMsgReq* msg, PTP::Group::GroupUnreadMsgRes* msg_rsp, PTP::Common::ERR& error){
    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn("group");
    while (true) {
        if (!pCacheConn) {
            error = PTP::Common:: E_SYSTEM;
            log_error("error pCacheConn");
            break;
        }
        string fromUserId = to_string(msg->auth_uid());
        list<string> argv;
        list<string> list_ret;
        argv.emplace_back("ZRANGE");
        argv.push_back(MEMBER_GROUP_UNREAD_KEY_PREFIX+fromUserId);
        argv.emplace_back("0");
        argv.emplace_back("+INF");
        argv.emplace_back("BYSCORE");
        argv.emplace_back("WITHSCORES");
        pCacheConn->exec(&argv,&list_ret);
        uint32_t total = 0;
        list<string> argv_group_msg;
        list<string> list_group_msg;
        argv_group_msg.emplace_back("MGET");
        for(auto it=list_ret.begin();it != list_ret.end();it++){
            advance(it,0);
            string group_id_str = it->c_str();
            advance(it,1);
            uint32_t unread_cnt = string2int(it->c_str());
            total += unread_cnt;
            if(unread_cnt > 0){
                argv_group_msg.push_back(GROUP_LATEST_MSG_DATA_BY_ID_PREFIX+group_id_str);
            }
        }

        pCacheConn->exec(&argv_group_msg,&list_group_msg);
        for(auto it=list_group_msg.begin();it != list_group_msg.end();it++){
            PTP::Common::MsgInfo * msg_info = msg_rsp->add_unread_list();
            string msg_info_hex = it->c_str();
            msg_info->ParseFromString(hex_to_string(msg_info_hex.substr(2)));
        }
        msg_rsp->set_unread_cnt(total);
        break;
    }

    if (pCacheConn) {
        pCacheManager->RelCacheConn(pCacheConn);
    }
}


void CModelGroup::GroupRemoveSessionReq(PTP::Group::GroupRemoveSessionReq* msg, PTP::Group::GroupRemoveSessionRes* msg_rsp, ERR& error)
{
    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn("group");

    while (true) {
        if (!pCacheConn) {
            error = PTP::Common::E_SYSTEM;
            log_error("error pCacheConn");
            break;
        }
        log("GroupRemoveSessionReq...");
        // auto fromUserId = msg->auth_uid();
        break;
    }

    if (pCacheConn) {
        pCacheManager->RelCacheConn(pCacheConn);
    }
}

void CModelGroup::GroupChangeMemberReq(PTP::Group::GroupChangeMemberReq* msg, PTP::Group::GroupChangeMemberRes* msg_rsp, ERR& error)
{
    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn("group");

    while (true) {
        if (!pCacheConn) {
            error = PTP::Common::E_SYSTEM;
            log_error("error pCacheConn");
            break;
        }
        log("GroupChangeMemberReq...");
        // auto fromUserId = msg->auth_uid();
        break;
    }

    if (pCacheConn) {
        pCacheManager->RelCacheConn(pCacheConn);
    }
}
