#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/AuthLoginAction.h"
#include "ptp_server/actions/AuthCaptchaAction.h"
#include "ptp_server/actions/ServerLoginAction.h"
#include "PTP.Auth.pb.h"
#include "PTP.Server.pb.h"

using namespace PTP::Common;

TEST(test_Auth, AuthLoginAction) {
    test_int();
    PTP::Auth::AuthCaptchaReq msg;
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg,CID_AuthCaptchaReq,sep_no);
    CRequest request;
    request.SetHandle(time(nullptr));
    auto pMsgConn = new CMsgSrvConn();
    addMsgSrvConnByHandle(request.GetHandle(),pMsgConn);
    request.SetRequestPdu(&pdu);
    ACTION_AUTH::AuthCaptchaReqAction(&request);
    if(request.GetResponsePdu()){
        ASSERT_EQ(request.GetResponsePdu()->GetSeqNum(),sep_no);
    }
    ASSERT_EQ(request.GetResponsePdu()->GetCommandId(),CID_AuthCaptchaRes);
    PTP::Auth::AuthCaptchaRes msg_rsp;
    auto res = msg_rsp.ParseFromArray(request.GetResponsePdu()->GetBodyData(), (int)request.GetResponsePdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_rsp.error();
    ASSERT_EQ(error,NO_ERROR);
    auto captcha = msg_rsp.captcha();
    ASSERT_EQ(captcha.size(),6);
    DEBUG_I("captcha: %s",captcha.c_str());
    auto address = bytes_to_hex_string((unsigned char *)msg_rsp.address().data(),20);
    ASSERT_EQ(msg_rsp.address().size(),20);
    DEBUG_I("address: %s", address.c_str());
    auto iv = msg_rsp.iv();
    ASSERT_EQ(iv.size(),16);
    DEBUG_I("iv: %s", bytes_to_hex_string((unsigned char *)iv.data(), 16).c_str());
    auto aad = msg_rsp.aad();
    ASSERT_EQ(aad.size(),16);
    DEBUG_I("add: %s", bytes_to_hex_string((unsigned char *)aad.data(), 16).c_str());
    auto sign = msg_rsp.sign();
    ASSERT_EQ(sign.size(),65);
    DEBUG_I("sign: %s", bytes_to_hex_string((unsigned char *)sign.data(), 65).c_str());

    PTP::Auth::AuthLoginReq msg_login;
    msg_login.set_address(address);
    msg_login.set_captcha(captcha);
    msg_login.set_client_type(PTP::Common::CLIENT_TYPE_ANDROID);
    msg_login.set_client_version("android/1.1");
    msg_login.set_sign(sign);
    ImPdu pdu_login;
    sep_no+=1;
    pdu_login.SetPBMsg(&msg_login,CID_AuthLoginReq,sep_no);
    CRequest request_login;
    request_login.SetHandle(request.GetHandle());
    request_login.SetRequestPdu(&pdu_login);
    ACTION_AUTH::AuthLoginReqAction(&request_login);
    if(request_login.GetResponsePdu()){
        ASSERT_EQ(request_login.GetResponsePdu()->GetSeqNum(),sep_no);
    }
    ASSERT_EQ(request_login.GetResponsePdu()->GetCommandId(),CID_ServerLoginReq);
    PTP::Server::ServerLoginReq msg_login_rsp;
    res = msg_login_rsp.ParseFromArray(request_login.GetResponsePdu()->GetBodyData(), (int)request_login.GetResponsePdu()->GetBodyLength());


    ASSERT_EQ(pMsgConn->IsOpen(),false);
    ASSERT_EQ(pMsgConn->GetAddress(),address);
    ASSERT_EQ(pMsgConn->GetUserId()>0,false);

    ASSERT_EQ(res,true);
    ASSERT_EQ(msg_login_rsp.captcha(),msg_login.captcha());
    ASSERT_EQ(
            msg_login_rsp.address(),
            address);

    ASSERT_EQ(
            bytes_to_hex_string((unsigned char *)msg_login_rsp.sign().data(),65),
            bytes_to_hex_string((unsigned char *)msg_login.sign().data(),65));

    ImPdu pdu_1;
    pdu_1.SetPBMsg(&msg_login_rsp,CID_ServerLoginReq,sep_no);
    CRequest request_1;
    request_1.SetIsBusinessConn(true);
    request_1.SetRequestPdu(&pdu_1);
    ACTION_SERVER::ServerLoginReqAction(&request_1);
    if(request_1.GetResponsePdu()){
        ASSERT_EQ(request_1.GetResponsePdu()->GetSeqNum(),sep_no);
    }
    ASSERT_EQ(request_1.GetResponsePdu()->GetCommandId(),CID_ServerLoginRes);
    PTP::Server::ServerLoginRes msg_login_res_rsp;
    res = msg_login_res_rsp.ParseFromArray(request_1.GetResponsePdu()->GetBodyData(), (int)request_1.GetResponsePdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    error = msg_login_res_rsp.error();
    ASSERT_EQ(error,NO_ERROR);
    ASSERT_EQ(true,msg_login_res_rsp.user_info().uid() > 0);
    DEBUG_I("address: %s",msg_login_res_rsp.user_info().address().c_str());
    ASSERT_EQ(
            msg_login_res_rsp.user_info().address(),
            address);

    ImPdu pdu_2;
    pdu_2.SetPBMsg(&msg_login_res_rsp,CID_ServerLoginRes,sep_no);
    CRequest request_2;
    request_2.SetHandle(request.GetHandle());
    request_2.SetIsBusinessConn(false);
    request_2.SetRequestPdu(&pdu_2);
    ACTION_SERVER::ServerLoginResAction(&request_2);
    if(request_2.GetResponsePdu()){
        ASSERT_EQ(request_2.GetResponsePdu()->GetSeqNum(),sep_no);
    }
    PTP::Auth::AuthLoginRes msg_login_res_rsp2;
    res = msg_login_res_rsp2.ParseFromArray(request_2.GetResponsePdu()->GetBodyData(), (int)request_2.GetResponsePdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    error = msg_login_res_rsp2.error();
    ASSERT_EQ(error,NO_ERROR);
    ASSERT_EQ(true,msg_login_res_rsp2.user_info().uid() > 0);
    ASSERT_EQ(request_2.GetResponsePdu()->GetCommandId(),CID_AuthLoginRes);
    ASSERT_EQ(pMsgConn->IsOpen(),true);
    ASSERT_EQ(pMsgConn->GetAddress(),address);
    ASSERT_EQ(pMsgConn->GetUserId(),msg_login_res_rsp2.user_info().uid());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
