#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/FileImgUploadAction.h"
#include "ptp_server/actions/models/ModelFile.h"
#include "PTP.File.pb.h"

using namespace PTP::Common;

TEST(test_File, FileImgUploadAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::File::FileImgUploadReq msg_FileImgUploadReq;
    msg_FileImgUploadReq.set_file_id("id");
    msg_FileImgUploadReq.set_file_part(1);
    msg_FileImgUploadReq.set_file_total_parts(1);
    char f[1];
    msg_FileImgUploadReq.set_file_data(f,1);
    uint16_t sep_no = 1;
    ImPdu pdu_FileImgUploadReq;
    pdu_FileImgUploadReq.SetPBMsg(&msg_FileImgUploadReq,CID_FileImgUploadReq,sep_no);
    CRequest request_FileImgUploadReq;
    request_FileImgUploadReq.SetHandle(pMsgConn->GetHandle());
    request_FileImgUploadReq.SetRequestPdu(&pdu_FileImgUploadReq);
    
    ACTION_FILE::FileImgUploadReqAction(&request_FileImgUploadReq);
    
    if(request_FileImgUploadReq.HasNext()){
        auto pdu_next_FileImgUploadReq = request_FileImgUploadReq.GetNextResponsePdu();
        ASSERT_EQ(pdu_next_FileImgUploadReq->GetSeqNum(),sep_no);
        ASSERT_EQ(pdu_next_FileImgUploadReq->GetCommandId(),CID_FileImgUploadReq);

        CRequest request_next_FileImgUploadReq;
        request_next_FileImgUploadReq.SetIsBusinessConn(true);
        request_next_FileImgUploadReq.SetRequestPdu(pdu_next_FileImgUploadReq);
        
        ACTION_FILE::FileImgUploadReqAction(&request_next_FileImgUploadReq);

        if(request_next_FileImgUploadReq.HasNext()){}
        if(request_next_FileImgUploadReq.GetResponsePdu()){
            PTP::File::FileImgUploadRes msg_FileImgUploadRes;
            auto res = msg_FileImgUploadRes.ParseFromArray(request_next_FileImgUploadReq.GetResponsePdu()->GetBodyData(), (int)request_next_FileImgUploadReq.GetResponsePdu()->GetBodyLength());
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_FileImgUploadReq.GetResponsePdu()->GetCommandId(),CID_FileImgUploadRes);
            auto error = msg_FileImgUploadRes.error();
            ASSERT_EQ(error,NO_ERROR);
            //auto file_path = msg_FileImgUploadRes.file_path();
            //DEBUG_I("file_path: %s",file_path.c_str());
            //auto auth_uid = msg_FileImgUploadRes.auth_uid();
            //DEBUG_I("auth_uid: %d",auth_uid);
            
            CRequest request_FileImgUploadRes;
            request_FileImgUploadRes.SetIsBusinessConn(false);
            request_FileImgUploadRes.SetHandle(pMsgConn->GetHandle());
            request_FileImgUploadRes.SetRequestPdu(request_next_FileImgUploadReq.GetResponsePdu());
            
            ACTION_FILE::FileImgUploadResAction(&request_FileImgUploadRes);
            
            if(request_FileImgUploadRes.HasNext()){}
            if(request_FileImgUploadRes.GetResponsePdu()){
                PTP::File::FileImgUploadRes msg_final_FileImgUploadRes;
                res = msg_final_FileImgUploadRes.ParseFromArray(request_FileImgUploadRes.GetResponsePdu()->GetBodyData(), (int)request_FileImgUploadRes.GetResponsePdu()->GetBodyLength());
                ASSERT_EQ(res,true);
                ASSERT_EQ(request_FileImgUploadRes.GetResponsePdu()->GetCommandId(),CID_FileImgUploadRes);
                error = msg_final_FileImgUploadRes.error();
                ASSERT_EQ(error,NO_ERROR);
            }
        }
      }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
