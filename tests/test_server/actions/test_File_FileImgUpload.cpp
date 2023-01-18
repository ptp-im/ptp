#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/FileImgUploadAction.h"
#include "ptp_server/actions/models/ModelFile.h"
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
    ImPdu pdu_FileImgUploadReq;
    pdu_FileImgUploadReq.SetPBMsg(&msg_FileImgUploadReq,CID_FileImgUploadReq,sep_no);
    CRequest request_FileImgUploadReq;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_FileImgUploadReq.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_FileImgUploadReq.GetHandle(),pMsgConn);
    request_FileImgUploadReq.SetRequestPdu(&pdu_FileImgUploadReq);
    ACTION_FILE::FileImgUploadReqAction(&request_FileImgUploadReq);
    if(request_FileImgUploadReq.GetResponsePdu()){
        ASSERT_EQ(request_FileImgUploadReq.GetResponsePdu()->GetSeqNum(),sep_no);
        ASSERT_EQ(request_FileImgUploadReq.GetResponsePdu()->GetCommandId(),CID_FileImgUploadReq);
        ASSERT_EQ(request_FileImgUploadReq.IsNext(),true);

        CRequest request_next_FileImgUploadReq;
        request_next_FileImgUploadReq.SetIsBusinessConn(true);
        request_next_FileImgUploadReq.SetRequestPdu(request_FileImgUploadReq.GetResponsePdu());
        ACTION_FILE::FileImgUploadReqAction(&request_next_FileImgUploadReq);

        PTP::File::FileImgUploadRes msg_FileImgUploadRes;
        auto res = msg_FileImgUploadRes.ParseFromArray(request_next_FileImgUploadReq.GetResponsePdu()->GetBodyData(), (int)request_next_FileImgUploadReq.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_next_FileImgUploadReq.GetResponsePdu()->GetCommandId(),CID_FileImgUploadRes);
        auto error = msg_FileImgUploadRes.error();
        ASSERT_EQ(error,NO_ERROR);
        //auto file_path = msg_FileImgUploadRes.file_path();
        //DEBUG_I("file_path: %s",file_path.c_str());
        //auto attach_data = msg_FileImgUploadRes.attach_data();
        //DEBUG_I("attach_data: %s",attach_data.c_str());
        //auto auth_uid = msg_FileImgUploadRes.auth_uid();
        //DEBUG_I("auth_uid: %d",auth_uid);
        
        CRequest request_FileImgUploadRes;
        request_FileImgUploadRes.SetIsBusinessConn(false);
        request_FileImgUploadRes.SetRequestPdu(request_next_FileImgUploadReq.GetResponsePdu());
        ACTION_FILE::FileImgUploadResAction(&request_FileImgUploadRes);
        PTP::File::FileImgUploadRes msg_final_FileImgUploadRes;
        res = msg_final_FileImgUploadRes.ParseFromArray(request_FileImgUploadRes.GetResponsePdu()->GetBodyData(), (int)request_FileImgUploadRes.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_FileImgUploadRes.GetResponsePdu()->GetCommandId(),CID_FileImgUploadRes);
        error = msg_final_FileImgUploadRes.error();
        ASSERT_EQ(error,NO_ERROR);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
