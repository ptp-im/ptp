/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ActionCommands.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 22, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __ActionCommands_H__
#define __ActionCommands_H__

#include <string>

using namespace std;

enum ActionCommands {
  CID_AuthCaptchaReq = 1001,
  CID_AuthCaptchaRes = 1002,
  CID_AuthLoginReq = 1003,
  CID_AuthLoginRes = 1004,
  CID_AuthLogoutReq = 1005,
  CID_AuthLogoutRes = 1006,
  CID_BuddyGetALLReq = 2001,
  CID_BuddyGetALLRes = 2002,
  CID_BuddyGetListReq = 2003,
  CID_BuddyGetListRes = 2004,
  CID_BuddyGetStatReq = 2005,
  CID_BuddyGetStatRes = 2006,
  CID_BuddyImportContactsReq = 2007,
  CID_BuddyImportContactsRes = 2008,
  CID_BuddyModifyNotify = 2009,
  CID_BuddyModifyReq = 2010,
  CID_BuddyModifyRes = 2011,
  CID_BuddyModifyUpdatePairNotify = 2012,
  CID_BuddyQueryListReq = 2013,
  CID_BuddyQueryListRes = 2014,
  CID_BuddyStatNotify = 2015,
  CID_FileImgDownloadReq = 3001,
  CID_FileImgDownloadRes = 3002,
  CID_FileImgUploadReq = 3003,
  CID_FileImgUploadRes = 3004,
  CID_GroupChangeMemberNotify = 4001,
  CID_GroupChangeMemberReq = 4002,
  CID_GroupChangeMemberRes = 4003,
  CID_GroupCreateReq = 4004,
  CID_GroupCreateRes = 4005,
  CID_GroupGetListReq = 4006,
  CID_GroupGetListRes = 4007,
  CID_GroupGetMembersListReq = 4008,
  CID_GroupGetMembersListRes = 4009,
  CID_GroupModifyNotify = 4010,
  CID_GroupModifyReq = 4011,
  CID_GroupModifyRes = 4012,
  CID_GroupPreCreateReq = 4013,
  CID_GroupPreCreateRes = 4014,
  CID_GroupUnreadMsgReq = 4015,
  CID_GroupUnreadMsgRes = 4016,
  CID_MsgGetByIdsReq = 5001,
  CID_MsgGetByIdsRes = 5002,
  CID_MsgNotify = 5003,
  CID_MsgReadAckReq = 5004,
  CID_MsgReadNotify = 5005,
  CID_MsgReq = 5006,
  CID_MsgRes = 5007,
  CID_HeartBeatNotify = 6001,
  CID_ServerLoginReq = 7001,
  CID_ServerLoginRes = 7002,
  CID_SwitchDevicesNotify = 8001,
  CID_SwitchDevicesReq = 8002,
  CID_SwitchPtpNotify = 8003,
  CID_SwitchPtpReq = 8004,
  CID_SwitchPtpRes = 8005,
};

string getActionCommandsName(ActionCommands cid);

#endif /* __ActionCommands_H__ */
