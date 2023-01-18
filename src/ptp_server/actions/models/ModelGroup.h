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
    static void updateGroupMemberPairUpdate(CacheConn *pCacheConnGroup,uint32_t group_id,uint32_t from_uid,uint32_t to_uid,uint32_t created_time);
    static uint32_t getGroupPairRelation(CacheConn *pCacheConnGroup,uint32_t first_uid,uint32_t second_uid);
    static void updateGroupPairRelation(CacheConn *pCacheConnGroup,uint32_t group_id,uint32_t from_uid,uint32_t to_uid);
    static void updateGroupMembers(CacheConn *pCacheConnGroup,list<string> &group_member_ids,uint32_t group_id,uint32_t created_time);
    static uint32_t getGroupIdByAddress(CacheConn *pCacheConnGroup,const string& group_address);
    static void getGroupInfoById(CacheConn *pCacheConnGroup,GroupInfo *group,uint32_t group_id);
    static uint32_t genGroupId(CacheConn *pCacheConnGroup);
    static uint32_t genGroupIdx(CacheConn *pCacheConnGroup,uint32_t from_uid);
    static void handleGroupSetCacheArgv(list<string> &set_argv,GroupInfo *group);
    static void handleGroupInfoCache(GroupInfo *group,list<string>::iterator it);
    static void handleGroupGetCacheArgv(list<string> &get_argv,uint32_t group_id);
private:
    CModelGroup();
private:
    static CModelGroup*    m_pInstance;

};

#endif /* MODEL_GROUP_H_ */
