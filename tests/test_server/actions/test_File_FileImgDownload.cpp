#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/FileImgDownloadAction.h"
#include "ptp_server/actions/models/ModelFile.h"
#include "PTP.File.pb.h"

using namespace PTP::Common;

TEST(test_File, FileImgDownloadAction) {
    test_int();
    PTP::File::FileImgDownloadReq msg_FileImgDownloadReq;
    //msg_FileImgDownloadReq.set_file_path();
    //msg_FileImgDownloadReq.set_attach_data();
    //msg_FileImgDownloadReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu_FileImgDownloadReq;
    pdu_FileImgDownloadReq.SetPBMsg(&msg_FileImgDownloadReq,CID_FileImgDownloadReq,sep_no);
    CRequest request_FileImgDownloadReq;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_FileImgDownloadReq.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_FileImgDownloadReq.GetHandle(),pMsgConn);
    request_FileImgDownloadReq.SetRequestPdu(&pdu_FileImgDownloadReq);
    ACTION_FILE::FileImgDownloadReqAction(&request_FileImgDownloadReq);
    if(request_FileImgDownloadReq.GetResponsePdu()){
        ASSERT_EQ(request_FileImgDownloadReq.GetResponsePdu()->GetSeqNum(),sep_no);
        ASSERT_EQ(request_FileImgDownloadReq.GetResponsePdu()->GetCommandId(),CID_FileImgDownloadReq);
        ASSERT_EQ(request_FileImgDownloadReq.IsNext(),true);

        CRequest request_next_FileImgDownloadReq;
        request_next_FileImgDownloadReq.SetIsBusinessConn(true);
        request_next_FileImgDownloadReq.SetRequestPdu(request_FileImgDownloadReq.GetResponsePdu());
        ACTION_FILE::FileImgDownloadReqAction(&request_next_FileImgDownloadReq);

        PTP::File::FileImgDownloadRes msg_FileImgDownloadRes;
        auto res = msg_FileImgDownloadRes.ParseFromArray(request_next_FileImgDownloadReq.GetResponsePdu()->GetBodyData(), (int)request_next_FileImgDownloadReq.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_next_FileImgDownloadReq.GetResponsePdu()->GetCommandId(),CID_FileImgDownloadRes);
        auto error = msg_FileImgDownloadRes.error();
        ASSERT_EQ(error,NO_ERROR);
        //auto file_data = msg_FileImgDownloadRes.file_data();
        //DEBUG_I("file_data: %s",file_data.c_str());
        //auto attach_data = msg_FileImgDownloadRes.attach_data();
        //DEBUG_I("attach_data: %s",attach_data.c_str());
        //auto auth_uid = msg_FileImgDownloadRes.auth_uid();
        //DEBUG_I("auth_uid: %d",auth_uid);
        
        CRequest request_FileImgDownloadRes;
        request_FileImgDownloadRes.SetIsBusinessConn(false);
        request_FileImgDownloadRes.SetRequestPdu(request_next_FileImgDownloadReq.GetResponsePdu());
        ACTION_FILE::FileImgDownloadResAction(&request_FileImgDownloadRes);
        PTP::File::FileImgDownloadRes msg_final_FileImgDownloadRes;
        res = msg_final_FileImgDownloadRes.ParseFromArray(request_FileImgDownloadRes.GetResponsePdu()->GetBodyData(), (int)request_FileImgDownloadRes.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_FileImgDownloadRes.GetResponsePdu()->GetCommandId(),CID_FileImgDownloadRes);
        error = msg_final_FileImgDownloadRes.error();
        ASSERT_EQ(error,NO_ERROR);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
