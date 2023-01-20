/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ActionCommands.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Thursday, January 19, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __ActionCommands_H__
#define __ActionCommands_H__
  
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
  CID_GroupRemoveSessionNotify = 4015,
  CID_GroupRemoveSessionReq = 4016,
  CID_GroupRemoveSessionRes = 4017,
  CID_GroupUnreadMsgReq = 4018,
  CID_GroupUnreadMsgRes = 4019,
  CID_MsgGetByIdsReq = 5001,
  CID_MsgGetByIdsRes = 5002,
  CID_MsgGetMaxIdReq = 5003,
  CID_MsgGetMaxIdRes = 5004,
  CID_MsgNotify = 5005,
  CID_MsgReadAckReq = 5006,
  CID_MsgReadNotify = 5007,
  CID_MsgReq = 5008,
  CID_MsgRes = 5009,
  CID_MsgUnNotify = 5010,
  CID_CaptchaReq = 6001,
  CID_CaptchaRes = 6002,
  CID_HeartBeatNotify = 6003,
  CID_ServerLoginReq = 7001,
  CID_ServerLoginRes = 7002,
  CID_SwitchDevicesNotify = 8001,
  CID_SwitchDevicesReq = 8002,
  CID_SwitchPtpNotify = 8003,
  CID_SwitchPtpReq = 8004,
  CID_SwitchPtpRes = 8005,
};

static string getActionCommandsName(ActionCommands cid){
    switch (cid) {
        case CID_AuthCaptchaReq:
            return to_string(cid) + ":CID_AuthCaptchaReq";
        case CID_AuthCaptchaRes:
            return to_string(cid) + ":CID_AuthCaptchaRes";
        case CID_AuthLoginReq:
            return to_string(cid) + ":CID_AuthLoginReq";
        case CID_AuthLoginRes:
            return to_string(cid) + ":CID_AuthLoginRes";
        case CID_AuthLogoutReq:
            return to_string(cid) + ":CID_AuthLogoutReq";
        case CID_AuthLogoutRes:
            return to_string(cid) + ":CID_AuthLogoutRes";
        case CID_BuddyGetALLReq:
            return to_string(cid) + ":CID_BuddyGetALLReq";
        case CID_BuddyGetALLRes:
            return to_string(cid) + ":CID_BuddyGetALLRes";
        case CID_BuddyGetListReq:
            return to_string(cid) + ":CID_BuddyGetListReq";
        case CID_BuddyGetListRes:
            return to_string(cid) + ":CID_BuddyGetListRes";
        case CID_BuddyGetStatReq:
            return to_string(cid) + ":CID_BuddyGetStatReq";
        case CID_BuddyGetStatRes:
            return to_string(cid) + ":CID_BuddyGetStatRes";
        case CID_BuddyImportContactsReq:
            return to_string(cid) + ":CID_BuddyImportContactsReq";
        case CID_BuddyImportContactsRes:
            return to_string(cid) + ":CID_BuddyImportContactsRes";
        case CID_BuddyModifyNotify:
            return to_string(cid) + ":CID_BuddyModifyNotify";
        case CID_BuddyModifyReq:
            return to_string(cid) + ":CID_BuddyModifyReq";
        case CID_BuddyModifyRes:
            return to_string(cid) + ":CID_BuddyModifyRes";
        case CID_BuddyModifyUpdatePairNotify:
            return to_string(cid) + ":CID_BuddyModifyUpdatePairNotify";
        case CID_BuddyQueryListReq:
            return to_string(cid) + ":CID_BuddyQueryListReq";
        case CID_BuddyQueryListRes:
            return to_string(cid) + ":CID_BuddyQueryListRes";
        case CID_BuddyStatNotify:
            return to_string(cid) + ":CID_BuddyStatNotify";
        case CID_FileImgDownloadReq:
            return to_string(cid) + ":CID_FileImgDownloadReq";
        case CID_FileImgDownloadRes:
            return to_string(cid) + ":CID_FileImgDownloadRes";
        case CID_FileImgUploadReq:
            return to_string(cid) + ":CID_FileImgUploadReq";
        case CID_FileImgUploadRes:
            return to_string(cid) + ":CID_FileImgUploadRes";
        case CID_GroupChangeMemberNotify:
            return to_string(cid) + ":CID_GroupChangeMemberNotify";
        case CID_GroupChangeMemberReq:
            return to_string(cid) + ":CID_GroupChangeMemberReq";
        case CID_GroupChangeMemberRes:
            return to_string(cid) + ":CID_GroupChangeMemberRes";
        case CID_GroupCreateReq:
            return to_string(cid) + ":CID_GroupCreateReq";
        case CID_GroupCreateRes:
            return to_string(cid) + ":CID_GroupCreateRes";
        case CID_GroupGetListReq:
            return to_string(cid) + ":CID_GroupGetListReq";
        case CID_GroupGetListRes:
            return to_string(cid) + ":CID_GroupGetListRes";
        case CID_GroupGetMembersListReq:
            return to_string(cid) + ":CID_GroupGetMembersListReq";
        case CID_GroupGetMembersListRes:
            return to_string(cid) + ":CID_GroupGetMembersListRes";
        case CID_GroupModifyNotify:
            return to_string(cid) + ":CID_GroupModifyNotify";
        case CID_GroupModifyReq:
            return to_string(cid) + ":CID_GroupModifyReq";
        case CID_GroupModifyRes:
            return to_string(cid) + ":CID_GroupModifyRes";
        case CID_GroupPreCreateReq:
            return to_string(cid) + ":CID_GroupPreCreateReq";
        case CID_GroupPreCreateRes:
            return to_string(cid) + ":CID_GroupPreCreateRes";
        case CID_GroupRemoveSessionNotify:
            return to_string(cid) + ":CID_GroupRemoveSessionNotify";
        case CID_GroupRemoveSessionReq:
            return to_string(cid) + ":CID_GroupRemoveSessionReq";
        case CID_GroupRemoveSessionRes:
            return to_string(cid) + ":CID_GroupRemoveSessionRes";
        case CID_GroupUnreadMsgReq:
            return to_string(cid) + ":CID_GroupUnreadMsgReq";
        case CID_GroupUnreadMsgRes:
            return to_string(cid) + ":CID_GroupUnreadMsgRes";
        case CID_MsgGetByIdsReq:
            return to_string(cid) + ":CID_MsgGetByIdsReq";
        case CID_MsgGetByIdsRes:
            return to_string(cid) + ":CID_MsgGetByIdsRes";
        case CID_MsgGetMaxIdReq:
            return to_string(cid) + ":CID_MsgGetMaxIdReq";
        case CID_MsgGetMaxIdRes:
            return to_string(cid) + ":CID_MsgGetMaxIdRes";
        case CID_MsgNotify:
            return to_string(cid) + ":CID_MsgNotify";
        case CID_MsgReadAckReq:
            return to_string(cid) + ":CID_MsgReadAckReq";
        case CID_MsgReadNotify:
            return to_string(cid) + ":CID_MsgReadNotify";
        case CID_MsgReq:
            return to_string(cid) + ":CID_MsgReq";
        case CID_MsgRes:
            return to_string(cid) + ":CID_MsgRes";
        case CID_MsgUnNotify:
            return to_string(cid) + ":CID_MsgUnNotify";
        case CID_CaptchaReq:
            return to_string(cid) + ":CID_CaptchaReq";
        case CID_CaptchaRes:
            return to_string(cid) + ":CID_CaptchaRes";
        case CID_HeartBeatNotify:
            return to_string(cid) + ":CID_HeartBeatNotify";
        case CID_ServerLoginReq:
            return to_string(cid) + ":CID_ServerLoginReq";
        case CID_ServerLoginRes:
            return to_string(cid) + ":CID_ServerLoginRes";
        case CID_SwitchDevicesNotify:
            return to_string(cid) + ":CID_SwitchDevicesNotify";
        case CID_SwitchDevicesReq:
            return to_string(cid) + ":CID_SwitchDevicesReq";
        case CID_SwitchPtpNotify:
            return to_string(cid) + ":CID_SwitchPtpNotify";
        case CID_SwitchPtpReq:
            return to_string(cid) + ":CID_SwitchPtpReq";
        case CID_SwitchPtpRes:
            return to_string(cid) + ":CID_SwitchPtpRes";
        default:
            return to_string(cid);
    }
}

#endif /* __ActionCommands_H__ */
