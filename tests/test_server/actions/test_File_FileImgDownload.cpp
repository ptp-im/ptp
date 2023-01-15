#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/FileImgDownloadAction.h"
#include "PTP.File.pb.h"

using namespace PTP::Common;

TEST(test_File, FileImgDownloadAction) {
    test_int();
    PTP::File::FileImgDownloadReq msg_FileImgDownloadReq;
    //msg_FileImgDownloadReq.set_file_path();
    //msg_FileImgDownloadReq.set_attach_data();
    //msg_FileImgDownloadReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_FileImgDownloadReq,CID_FileImgDownloadReq,sep_no);
    CRequest request_FileImgDownloadReq;
    CResponse response_FileImgDownloadReq;
    request_FileImgDownloadReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_FileImgDownloadReq.GetHandle(),new CMsgSrvConn());
    request_FileImgDownloadReq.SetPdu(&pdu);
    ACTION_FILE::FileImgDownloadReqAction(&request_FileImgDownloadReq,&response_FileImgDownloadReq);
    if(response_FileImgDownloadReq.GetPdu()){
        ASSERT_EQ(response_FileImgDownloadReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
    ASSERT_EQ(response_FileImgDownloadReq.GetPdu()->GetCommandId(),CID_FileImgDownloadRes);
    PTP::File::FileImgDownloadRes msg_FileImgDownloadRes;
    auto res = msg_FileImgDownloadRes.ParseFromArray(response_FileImgDownloadReq.GetPdu()->GetBodyData(), (int)response_FileImgDownloadReq.GetPdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_FileImgDownloadRes.error();
    ASSERT_EQ(error,NO_ERROR);
    //auto file_data = msg_FileImgDownloadReq.file_data();
    //DEBUG_I("file_data: %s",file_data.c_str());
    //auto attach_data = msg_FileImgDownloadReq.attach_data();
    //DEBUG_I("attach_data: %s",attach_data.c_str());
    //auto auth_uid = msg_FileImgDownloadReq.auth_uid();
    //DEBUG_I("auth_uid: %d",auth_uid);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
