/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupGetListAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "GroupGetListAction.h"
#include "PTP.Group.pb.h"
#include "MsgSrvConn.h"
#include "models/ModelGroup.h"

using namespace PTP::Common;

namespace ACTION_GROUP {
    void GroupGetListReqAction(CRequest* request){
        PTP::Group::GroupGetListReq msg; 
        PTP::Group::GroupGetListRes msg_rsp;
        ERR error = NO_ERROR;
        while (true){
            if(!msg.ParseFromArray(request->GetRequestPdu()->GetBodyData(), (int)request->GetRequestPdu()->GetBodyLength()))
            {
                error = E_PB_PARSE_ERROR;
                break;
            }
            if(!request->IsBusinessConn()){
              auto pMsgConn = FindMsgSrvConnByHandle(request->GetHandle());
              if(!pMsgConn){
                  DEBUG_E("not found pMsgConn");
                  return;
              }
             msg.set_auth_uid(pMsgConn->GetUserId());
             request->Next(&msg,CID_GroupGetListReq,request->GetSeqNum());
            }else{
                CacheManager *pCacheManager = CacheManager::getInstance();
                CacheConn *pCacheConn = pCacheManager->GetCacheConn(CACHE_GROUP_INSTANCE);
                while (true) {
                    if (!pCacheConn) {
                        error = PTP::Common:: E_SYSTEM;
                        DEBUG_E("error pCacheConn");
                        break;
                    }
                    auto auth_uid = msg.auth_uid();
                    list<uint32_t> group_ids;
                    list<uint32_t> removed_group_ids;
                    CModelGroup::getUpdatedGroups(pCacheConn,group_ids,removed_group_ids,auth_uid,msg.group_info_updated_time());
                    for(uint32_t group_id:group_ids){
                        GroupInfo *group = msg_rsp.add_groups();
                        CModelGroup::getGroupInfoById(pCacheConn,group,group_id);
                    }
                    for(uint32_t group_id : removed_group_ids){
                        msg_rsp.add_removed_group_ids(group_id);
                    }
                    msg_rsp.set_group_info_updated_time(time(nullptr));
                    msg_rsp.set_error(error);
                    msg_rsp.set_auth_uid(auth_uid);
                    break;
                }

                if (pCacheConn) {
                    pCacheManager->RelCacheConn(pCacheConn);
                }
                request->SendResponseMsg(&msg_rsp,CID_GroupGetListRes,request->GetSeqNum());
            }
            break;
        }
        if(error!= NO_ERROR){
            msg_rsp.set_error(error);
            request->SendResponseMsg(&msg_rsp,CID_GroupGetListRes,request->GetSeqNum());
        }
    }
    void GroupGetListResAction(CRequest* request){
        request->SendResponsePdu(request->GetRequestPdu());
    }
};

