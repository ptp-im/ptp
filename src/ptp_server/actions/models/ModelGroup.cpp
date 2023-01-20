/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelGroup.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
 *
================================================================*/
#include "ModelGroup.h"

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

uint32_t CModelGroup::getGroupIdByAddress(CacheConn *pCacheConnGroup,const string& group_address){
    auto group_id_str =  pCacheConnGroup->get(GROUP_ADDRESS_ID_PREFIX + group_address);
    return group_id_str.empty() ? 0 : (uint32_t)string2int(group_id_str);
}

uint32_t CModelGroup::genGroupId(CacheConn *pCacheConnGroup){
    auto group_id = pCacheConnGroup->incr(GROUP_ID_INCR_PREFIX);
    if(group_id == 1){
        group_id =  pCacheConnGroup->incrBy(GROUP_ID_INCR_PREFIX,599);
    }
    return group_id;
}

uint32_t CModelGroup::genGroupIdx(CacheConn *pCacheConnGroup,uint32_t from_uid){
    auto group_index_long = pCacheConnGroup->incr(GROUP_IDX_BY_UID_INCR_PREFIX + to_string(from_uid));
    return group_index_long;
}

void CModelGroup::updateGroupMemberPairUpdate(CacheConn *pCacheConnGroup,uint32_t from_uid,uint32_t to_uid,uint32_t updated_time){
    auto from_uid_str = to_string(from_uid);
    auto to_uid_str = to_string(to_uid);
    list<string> argv_member_pair_update;
    argv_member_pair_update.emplace_back("ZADD");
    argv_member_pair_update.push_back(MEMBER_PAIR_UPDATE_PREFIX+from_uid_str);
    argv_member_pair_update.push_back(to_string(updated_time));
    argv_member_pair_update.push_back(to_uid_str);
    pCacheConnGroup->exec(&argv_member_pair_update, nullptr);
}

void CModelGroup::updateGroupPairRelation(CacheConn *pCacheConnGroup,uint32_t group_id,uint32_t from_uid,uint32_t to_uid){
    string group_id_str = to_string(group_id);
    if (to_uid >= from_uid) {
        pCacheConnGroup->hset(GROUP_PAIR_USER_REL_PREFIX + to_string(from_uid), to_string(to_uid),
                         group_id_str);
    } else {
        pCacheConnGroup->hset(GROUP_PAIR_USER_REL_PREFIX + to_string(to_uid), to_string(from_uid),
                         group_id_str);
    }
}

uint32_t CModelGroup::getGroupPairRelation(CacheConn *pCacheConnGroup,uint32_t first_uid,uint32_t second_uid){
    string group_id_str;
    if (second_uid >= first_uid) {
        group_id_str = pCacheConnGroup->hget(GROUP_PAIR_USER_REL_PREFIX + to_string(first_uid), to_string(second_uid));
    } else {
        group_id_str = pCacheConnGroup->hget(GROUP_PAIR_USER_REL_PREFIX + to_string(second_uid), to_string(first_uid));
    }
    return group_id_str.empty() ? 0 : string2int(group_id_str);
}

void CModelGroup::updateGroupMembers(CacheConn *pCacheConnGroup,list<string> &group_member_ids,uint32_t group_id,uint32_t updated_time){
    string group_id_str = to_string(group_id);
    for (const auto& it : group_member_ids) {
        string member_id = it;
        list<string> member_group_argv;
        member_group_argv.push_back(group_id_str);

        pCacheConnGroup->cmd(
                "SADD",
                MEMBER_GROUP_KEY_PREFIX + member_id,
                &member_group_argv,
                nullptr);
    }
    updateGroupMembersUpdate(pCacheConnGroup,group_member_ids,group_id,updated_time);
    updateGroupMembersStatus(pCacheConnGroup,group_member_ids,group_id,PTP::Common::GROUP_MEMBER_STATUS_NORMAL);
}

void CModelGroup::updateGroupMembersUpdate(CacheConn *pCacheConnGroup,list<string> &group_member_ids,uint32_t group_id,uint32_t updated_time){
    list<string> group_member_update_argv;
    string group_id_str = to_string(group_id);
    for (const auto& it : group_member_ids) {
        string member_id = it;
        group_member_update_argv.push_back(to_string(updated_time));
        group_member_update_argv.push_back(member_id);
    }
    pCacheConnGroup->cmd(
            "ZADD",
            GROUP_MEMBER_UPDATE_KEY_PREFIX + group_id_str,
            &group_member_update_argv,
            nullptr);
}

void CModelGroup::updateGroupMembersStatus(CacheConn *pCacheConnGroup,list<string> &group_member_ids,uint32_t group_id,PTP::Common::GroupMemberStatus status){
    list<string> group_member_status_argv;
    string group_id_str = to_string(group_id);
    for (const auto& it : group_member_ids) {
        string member_id = it;
        group_member_status_argv.push_back(to_string(status));
        group_member_status_argv.push_back(member_id);
    }

    pCacheConnGroup->cmd(
            "ZADD",
            GROUP_MEMBER_STATUS_KEY_PREFIX + group_id_str,
            &group_member_status_argv,
            nullptr);
}

void CModelGroup::getGroupMembersStatus(CacheConn *pCacheConnGroup,list<string> &member_ids,uint32_t group_id,GroupMemberStatus status){
    list<string> argv;
    argv.emplace_back("ZRANGE");
    argv.push_back(GROUP_MEMBER_STATUS_KEY_PREFIX + to_string(group_id));
    argv.push_back(to_string(status));
    argv.emplace_back("+INF");
    argv.emplace_back("BYSCORE");
    pCacheConnGroup->exec(&argv,&member_ids);
}

void CModelGroup::getUserGroupPairs(CacheConn *pCacheConnGroup,list<string> &pair_user_ids,uint32_t uid,uint32_t updated_time){
    list<string> argv;
    argv.emplace_back("ZRANGE");
    argv.push_back(MEMBER_PAIR_UPDATE_PREFIX + to_string(uid));
    argv.push_back(to_string(updated_time));
    argv.emplace_back("+INF");
    argv.emplace_back("BYSCORE");
    pCacheConnGroup->exec(&argv,&pair_user_ids);
}

void CModelGroup::getMemberGroups(CacheConn *pCacheConnGroup,list<string> &group_ids,uint32_t uid){
    list<string> argv;
    argv.emplace_back("SMEMBERS");
    argv.push_back(MEMBER_GROUP_KEY_PREFIX + to_string(uid));
    pCacheConnGroup->exec(&argv,&group_ids);
}

void CModelGroup::getGroupMembersUpdated(CacheConn *pCacheConnGroup,list<string> &updated_member_ids,uint32_t group_id,uint32_t group_members_updated_time){
    list<string> argv;
    argv.emplace_back("ZRANGE");
    argv.push_back(GROUP_MEMBER_UPDATE_KEY_PREFIX + to_string(group_id));
    argv.push_back(to_string(group_members_updated_time));
    argv.emplace_back("+INF");
    argv.emplace_back("BYSCORE");
    pCacheConnGroup->exec(&argv,&updated_member_ids);
}

void CModelGroup::handleGroupSetCacheArgv(list<string> &set_argv,GroupInfo* group) {
    string group_id_str = to_string(group->group_id());
    string group_idx_str = to_string(group->group_idx());
    string group_type_str = to_string(group->group_type());
    set_argv.emplace_back("MSET");
    set_argv.push_back(GROUP_IDX_PREFIX + group_id_str);
    set_argv.push_back(group_idx_str);
    set_argv.push_back(GROUP_TYPE_PREFIX + group_id_str);
    set_argv.push_back(group_type_str);
    set_argv.push_back(GROUP_ADDRESS_PREFIX + group_id_str);
    set_argv.push_back(group->group_adr());
    set_argv.push_back(GROUP_ABOUT_PREFIX + group_id_str);
    set_argv.push_back(group->about());
    set_argv.push_back(GROUP_NAME_PREFIX + group_id_str);
    set_argv.push_back(group->name());
    set_argv.push_back(GROUP_AVATAR_PREFIX + group_id_str);
    set_argv.push_back(group->avatar());
    set_argv.push_back(GROUP_OWNER_PREFIX + group_id_str);
    set_argv.push_back(to_string(group->owner_uid()));
    set_argv.push_back(GROUP_PAIR_PREFIX + group_id_str);
    set_argv.push_back(to_string(group->pair_uid()));
    set_argv.push_back(GROUP_UPDATE_PREFIX + group_id_str);
    set_argv.push_back(to_string(group->updated_time()));
    set_argv.push_back(GROUP_CREATE_PREFIX + group_id_str);
    set_argv.push_back(to_string(group->created_time()));
}

void CModelGroup::handleGroupGetCacheArgv(list<string> &get_argv,uint32_t group_id) {
    string group_id_str = to_string(group_id);
    get_argv.push_back(GROUP_IDX_PREFIX + group_id_str);
    get_argv.push_back(GROUP_TYPE_PREFIX + group_id_str);
    get_argv.push_back(GROUP_ADDRESS_PREFIX + group_id_str);
    get_argv.push_back(GROUP_ABOUT_PREFIX + group_id_str);
    get_argv.push_back(GROUP_NAME_PREFIX + group_id_str);
    get_argv.push_back(GROUP_AVATAR_PREFIX + group_id_str);
    get_argv.push_back(GROUP_OWNER_PREFIX + group_id_str);
    get_argv.push_back(GROUP_PAIR_PREFIX + group_id_str);
    get_argv.push_back(GROUP_UPDATE_PREFIX + group_id_str);
    get_argv.push_back(GROUP_CREATE_PREFIX + group_id_str);
}


void CModelGroup::handleGroupInfoCache(GroupInfo *group,list<string>::iterator it) {
    group->set_group_idx(string2int(*it));
    advance(it,1);
    group->set_group_type((PTP::Common::GroupType)string2int(*it));
    advance(it,1);
    group->set_group_adr(it->c_str());
    advance(it,1);
    group->set_about(it->c_str());
    advance(it,1);
    group->set_name(it->c_str());
    advance(it,1);
    group->set_avatar(it->c_str());
    advance(it,1);
    group->set_owner_uid(string2int(*it));
    advance(it,1);
    group->set_pair_uid(string2int(*it));
    advance(it,1);
    group->set_updated_time(string2int(*it));
    advance(it,1);
    group->set_created_time(string2int(*it));
}

void CModelGroup::getGroupInfoById(CacheConn *pCacheConnGroup,GroupInfo *group,uint32_t group_id){
    list<string> get_argv;
    list<string> list_ret;
    get_argv.emplace_back("MGET");
    handleGroupGetCacheArgv(get_argv,group_id);
    pCacheConnGroup->exec(&get_argv,&list_ret);
    group->set_group_id(group_id);
    auto it = list_ret.begin();
    handleGroupInfoCache(group,it);
}