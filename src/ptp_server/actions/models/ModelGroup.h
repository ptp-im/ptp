/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelGroup.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：DO NOT EDIT!!
 *
 *
 ================================================================*/

#ifndef MODEL_GROUP_H_
#define MODEL_GROUP_H_

#include "PTP.Group.pb.h"
#include "../../CachePool.h"
#include <list>

using namespace std;
using namespace PTP::Common;

class CModelGroup {
public:
    virtual ~CModelGroup();
    static CModelGroup* getInstance();
    static void modify(CacheConn *pCacheConnGroup,GroupModifyAction action,const string &value,uint32_t group_id,ERR &error);
    static void getUserGroupPairs(CacheConn *pCacheConnGroup,list<string> &pair_user_ids,uint32_t uid,uint32_t updated_time = 0);
    static void updateGroupMemberPairUpdate(CacheConn *pCacheConnGroup,uint32_t from_uid,uint32_t to_uid,uint32_t updated_time);
    static uint32_t getGroupPairRelation(CacheConn *pCacheConnGroup,uint32_t first_uid,uint32_t second_uid);
    static void updateGroupPairRelation(CacheConn *pCacheConnGroup,uint32_t group_id,uint32_t from_uid,uint32_t to_uid);
    static void updateGroupMembersUpdate(CacheConn *pCacheConnGroup,list<string> &group_member_ids,uint32_t group_id,uint32_t updated_time);
    static void updateGroupMembersStatus(CacheConn *pCacheConnGroup,list<string> &group_member_ids,uint32_t group_id,PTP::Common::GroupMemberStatus status);
    static void updateGroupMembers(CacheConn *pCacheConnGroup,list<string> &group_member_ids,uint32_t group_id,uint32_t updated_time,GroupMemberStatus status);
    static uint32_t getGroupIdByAddress(CacheConn *pCacheConnGroup,const string& group_address);
    static void getGroupInfoById(CacheConn *pCacheConnGroup,GroupInfo *group,uint32_t group_id);
    static uint32_t genGroupId(CacheConn *pCacheConnGroup);
    static uint32_t genGroupIdx(CacheConn *pCacheConnGroup,uint32_t from_uid);
    static void cacheGroupInfo(CacheConn *pCacheConnGroup,GroupInfo *group);
    static void handleGroupInfoCache(GroupInfo *group,list<string>::iterator it);
    static void handleGroupGetCacheArgv(list<string> &get_argv,uint32_t group_id);
    static void getMemberGroupsByStatus(CacheConn *pCacheConnGroup,list<string> &group_ids,uint32_t uid,int16_t start_status,int16_t stop_status = 0,bool with_scores = false);
    static void getGroupMembersByStatus(CacheConn *pCacheConnGroup,list<string> &member_ids,uint32_t group_id,int16_t start_status,int16_t stop_status = 0,bool with_scores = false);
    static void getGroupMembersByUpdatedTime(CacheConn *pCacheConnGroup,list<string> &updated_member_ids,uint32_t group_id,uint32_t group_members_updated_time);
    static void getUpdatedGroups(CacheConn *pCacheConnGroup,list<uint32_t> &group_ids,list<uint32_t> &removed_group_ids,uint32_t uid,uint32_t updated_time);
private:
    CModelGroup();
private:
    static CModelGroup*    m_pInstance;
};

#endif /* MODEL_GROUP_H_ */
