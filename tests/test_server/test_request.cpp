#include <gtest/gtest.h>

#include "ptp_server/Request.h"
#include "ptp_protobuf/PTP.Auth.pb.h"


TEST(ptp_server, Request) {
    auto request = new CRequest();
    ImPdu pdu;
    ImPdu pdu_rsp;
    PTP::Auth::AuthCaptchaReq msg;
    PTP::Auth::AuthCaptchaRes msg_rsp;
    msg_rsp.set_address("address");
    pdu.SetPBMsg(&msg,CID_AuthCaptchaReq,2);
    request->SetRequestPdu(&pdu);

    ASSERT_EQ(request->GetRequestPdu()->GetCommandId(),CID_AuthCaptchaReq);
    ASSERT_EQ(request->GetRequestPdu()->GetSeqNum(),2);

    request->Next(&msg_rsp,CID_AuthCaptchaRes,2);
    ASSERT_EQ(request->GetResponsePdu()->GetCommandId(),CID_AuthCaptchaRes);
    ASSERT_EQ(request->GetResponsePdu()->GetSeqNum(),2);
    ASSERT_EQ(request->IsNext(),true);
    pdu_rsp.SetPBMsg(&msg_rsp,CID_AuthCaptchaRes,2);
    request->SendResponsePdu(&pdu_rsp);
    ASSERT_EQ(request->GetResponsePdu()->GetCommandId(),CID_AuthCaptchaRes);
    ASSERT_EQ(request->GetResponsePdu()->GetSeqNum(),2);
    ASSERT_EQ(request->IsNext(),false);
    auto handle = time(nullptr);
    request->SetHandle(handle);
    request->SetIsBusinessConn(true);
    ASSERT_TRUE(handle == request->GetHandle());
    ASSERT_TRUE(true == request->IsBusinessConn());

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}