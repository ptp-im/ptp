#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/FileImgUploadAction.h"
#include "PTP.File.pb.h"

using namespace PTP::Common;

TEST(test_File, FileImgUploadAction) {
    test_int();
    PTP::File::FileImgUploadReq msg_FileImgUploadReq;
    //msg_FileImgUploadReq.set_file_id();
    //msg_FileImgUploadReq.set_file_part();
    //msg_FileImgUploadReq.set_file_total_parts();
    //msg_FileImgUploadReq.set_file_data();
    //msg_FileImgUploadReq.set_attach_data();
    //msg_FileImgUploadReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_FileImgUploadReq,CID_FileImgUploadReq,sep_no);
    CRequest request_FileImgUploadReq;
    CResponse response_FileImgUploadReq;
    request_FileImgUploadReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_FileImgUploadReq.GetHandle(),new CMsgSrvConn());
    request_FileImgUploadReq.SetPdu(&pdu);
    ACTION_FILE::FileImgUploadReqAction(&request_FileImgUploadReq,&response_FileImgUploadReq);
    if(response_FileImgUploadReq.GetPdu()){
        ASSERT_EQ(response_FileImgUploadReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
    ASSERT_EQ(response_FileImgUploadReq.GetPdu()->GetCommandId(),CID_FileImgUploadRes);
    PTP::File::FileImgUploadRes msg_FileImgUploadRes;
    auto res = msg_FileImgUploadRes.ParseFromArray(response_FileImgUploadReq.GetPdu()->GetBodyData(), (int)response_FileImgUploadReq.GetPdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_FileImgUploadRes.error();
    ASSERT_EQ(error,NO_ERROR);
    //auto file_path = msg_FileImgUploadReq.file_path();
    //DEBUG_I("file_path: %s",file_path.c_str());
    //auto attach_data = msg_FileImgUploadReq.attach_data();
    //DEBUG_I("attach_data: %s",attach_data.c_str());
    //auto auth_uid = msg_FileImgUploadReq.auth_uid();
    //DEBUG_I("auth_uid: %d",auth_uid);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
