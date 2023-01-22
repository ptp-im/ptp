#include "ActionCommands.h"

string getActionCommandsName(ActionCommands cid){
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
        case CID_GroupUnreadMsgReq:
            return to_string(cid) + ":CID_GroupUnreadMsgReq";
        case CID_GroupUnreadMsgRes:
            return to_string(cid) + ":CID_GroupUnreadMsgRes";
        case CID_MsgGetByIdsReq:
            return to_string(cid) + ":CID_MsgGetByIdsReq";
        case CID_MsgGetByIdsRes:
            return to_string(cid) + ":CID_MsgGetByIdsRes";
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
