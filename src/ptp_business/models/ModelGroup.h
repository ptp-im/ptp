/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelGroup.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Tuesday, November 29, 2022
 *   desc：DO NOT EDIT!!
 *
 *
 ================================================================*/

#ifndef MODEL_GROUP_H_
#define MODEL_GROUP_H_

#include "PTP.Group.pb.h"
#include <list>

using namespace std;
using namespace PTP::Common;

class CModelGroup {
public:
virtual ~CModelGroup();

    static CModelGroup* getInstance();
    void GroupChangeMemberReq(PTP::Group::GroupChangeMemberReq* msg, PTP::Group::GroupChangeMemberRes* msg_rsp, ERR& error);
    void GroupCreateReq(PTP::Group::GroupCreateReq* msg, PTP::Group::GroupCreateRes* msg_rsp, ERR& error);
    void GroupGetListReq(PTP::Group::GroupGetListReq* msg, PTP::Group::GroupGetListRes* msg_rsp, ERR& error);
    void GroupGetMembersListReq(PTP::Group::GroupGetMembersListReq* msg, PTP::Group::GroupGetMembersListRes* msg_rsp, ERR& error);
    void GroupModifyReq(PTP::Group::GroupModifyReq* msg, PTP::Group::GroupModifyRes* msg_rsp, ERR& error);
    void GroupPreCreateReq(PTP::Group::GroupPreCreateReq* msg, PTP::Group::GroupPreCreateRes* msg_rsp, ERR& error);
    void GroupRemoveSessionReq(PTP::Group::GroupRemoveSessionReq* msg, PTP::Group::GroupRemoveSessionRes* msg_rsp, ERR& error);
    void GroupUnreadMsgReq(PTP::Group::GroupUnreadMsgReq* msg, PTP::Group::GroupUnreadMsgRes* msg_rsp, ERR& error);
    void getGroupMembers(uint32_t nGroupId, list<uint32_t>& lsUserId);
private:
    CModelGroup();
    
private:
    static CModelGroup*    m_pInstance;
};

#endif /* MODEL_GROUP_H_ */
