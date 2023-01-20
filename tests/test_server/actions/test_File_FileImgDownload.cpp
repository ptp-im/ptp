#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/FileImgDownloadAction.h"
#include "ptp_server/actions/models/ModelFile.h"
#include "PTP.File.pb.h"

using namespace PTP::Common;

TEST(test_File, FileImgDownloadAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::File::FileImgDownloadReq msg_FileImgDownloadReq;
    msg_FileImgDownloadReq.set_file_path("path");
    uint16_t sep_no = 1;
    ImPdu pdu_FileImgDownloadReq;
    pdu_FileImgDownloadReq.SetPBMsg(&msg_FileImgDownloadReq,CID_FileImgDownloadReq,sep_no);
    CRequest request_FileImgDownloadReq;
    request_FileImgDownloadReq.SetHandle(pMsgConn->GetHandle());
    request_FileImgDownloadReq.SetRequestPdu(&pdu_FileImgDownloadReq);
    
    ACTION_FILE::FileImgDownloadReqAction(&request_FileImgDownloadReq);
    
    if(request_FileImgDownloadReq.HasNext()){
        auto pdu_next_FileImgDownloadReq = request_FileImgDownloadReq.GetNextResponsePdu();
        ASSERT_EQ(pdu_next_FileImgDownloadReq->GetSeqNum(),sep_no);
        ASSERT_EQ(pdu_next_FileImgDownloadReq->GetCommandId(),CID_FileImgDownloadReq);

        CRequest request_next_FileImgDownloadReq;
        request_next_FileImgDownloadReq.SetIsBusinessConn(true);
        request_next_FileImgDownloadReq.SetRequestPdu(pdu_next_FileImgDownloadReq);
        
        ACTION_FILE::FileImgDownloadReqAction(&request_next_FileImgDownloadReq);

        if(request_next_FileImgDownloadReq.HasNext()){}
        if(request_next_FileImgDownloadReq.GetResponsePdu()){
            PTP::File::FileImgDownloadRes msg_FileImgDownloadRes;
            auto res = msg_FileImgDownloadRes.ParseFromArray(request_next_FileImgDownloadReq.GetResponsePdu()->GetBodyData(), (int)request_next_FileImgDownloadReq.GetResponsePdu()->GetBodyLength());
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_FileImgDownloadReq.GetResponsePdu()->GetCommandId(),CID_FileImgDownloadRes);
            auto error = msg_FileImgDownloadRes.error();
            ASSERT_EQ(error,NO_ERROR);
            //auto file_data = msg_FileImgDownloadRes.file_data();
            //DEBUG_I("file_data: %s",file_data.c_str());
            //auto auth_uid = msg_FileImgDownloadRes.auth_uid();
            //DEBUG_I("auth_uid: %d",auth_uid);
            
            CRequest request_FileImgDownloadRes;
            request_FileImgDownloadRes.SetIsBusinessConn(false);
            request_FileImgDownloadRes.SetHandle(pMsgConn->GetHandle());
            request_FileImgDownloadRes.SetRequestPdu(request_next_FileImgDownloadReq.GetResponsePdu());
            
            ACTION_FILE::FileImgDownloadResAction(&request_FileImgDownloadRes);
            
            if(request_FileImgDownloadRes.HasNext()){}
            if(request_FileImgDownloadRes.GetResponsePdu()){
                PTP::File::FileImgDownloadRes msg_final_FileImgDownloadRes;
                res = msg_final_FileImgDownloadRes.ParseFromArray(request_FileImgDownloadRes.GetResponsePdu()->GetBodyData(), (int)request_FileImgDownloadRes.GetResponsePdu()->GetBodyLength());
                ASSERT_EQ(res,true);
                ASSERT_EQ(request_FileImgDownloadRes.GetResponsePdu()->GetCommandId(),CID_FileImgDownloadRes);
                error = msg_final_FileImgDownloadRes.error();
                ASSERT_EQ(error,NO_ERROR);
            }
        }
      }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
