/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：UserAction.h
 *   创 建 者：PTP
 *   邮    箱：crypto.service@proton.me
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#ifndef __USER_ACTION_H__
#define __USER_ACTION_H__

#include "ImPduBase.h"

namespace DB_PROXY {
    void BuddyModifyUpdatePairProxy(CImPdu* pPdu, uint32_t conn_uuid);
    void BuddyGetALLReqProxy(CImPdu* pPdu, uint32_t conn_uuid);
    void BuddyGetListReqProxy(CImPdu* pPdu, uint32_t conn_uuid);
    void BuddyModifyReqProxy(CImPdu* pPdu, uint32_t conn_uuid);
    void BuddyQueryListReqProxy(CImPdu* pPdu, uint32_t conn_uuid);
    void getUserInfo(CImPdu* pPdu, uint32_t conn_uuid);
    void getUserInfo(CImPdu* pPdu, uint32_t conn_uuid);
    void getChangedUser(CImPdu* pPdu, uint32_t conn_uuid);
    void changeUserSignInfo(CImPdu* pPdu, uint32_t conn_uuid);
    void changeUserAvatarUrl(CImPdu* pPdu, uint32_t conn_uuid);
    void doPushShield(CImPdu* pPdu, uint32_t conn_uuid);
    void doQueryPushShield(CImPdu* pPdu, uint32_t conn_uuid);
};


#endif /* __USER_ACTION_H__ */
