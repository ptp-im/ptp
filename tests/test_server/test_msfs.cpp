#include <gtest/gtest.h>
#include <array>
#include "ptp_global/SocketClient.h"
#include "ptp_global/Helpers.h"
#include "ptp_global/Logger.h"
#include "ptp_protobuf/PTP.File.pb.h"
#include "ptp_protobuf/ImPdu.h"

using namespace std;


TEST(test_msfs, upload) {
    auto size = 1024 * 1024 * 1024;
    auto *pduBytes = new unsigned char[size]();
    for (int i = 0; i <size; ++i) {
        pduBytes[i] = i;
    }

    PTP::File::FileImgUploadReq msg;
    msg.set_file_data(pduBytes,size);
    msg.set_file_part(1);
    msg.set_file_total_parts(1);
    msg.set_file_id("1");
    ImPdu pdu;
    pdu.SetPBMsg(&msg,CID_FileImgUploadReq,0);
    pdu.Dump();
    CSimpleBuffer request = *new CSimpleBuffer();
    CSimpleBuffer response = *new CSimpleBuffer();
    string ip = "127.0.0.1";
    uint16_t port = 7841;
    request.Write((void *)pdu.GetBuffer(), pdu.GetLength());
    DEBUG_D("pduBytes size:%d",size);
    auto ret = socketSend(ip.c_str(),port,&request,&response);
    if(ret == SOCKET_CLIENT_OK){
        DEBUG_D("socketSend ret:%d",ret);
        delete[] pduBytes;

        ASSERT_EQ(response.GetLength()> 0, true);
        ImPdu *pPdu;
        pPdu = (ImPdu *)CImPdu::ReadPdu(response.GetBuffer(), response.GetLength());
        pPdu->Dump();
        ASSERT_EQ(pPdu->GetCommandId(), CID_FileImgUploadRes);
        PTP::File::FileImgUploadRes msg_rsp;
        msg_rsp.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength());
        DEBUG_D("error:%d",msg_rsp.error());
        DEBUG_D("path:%s",msg_rsp.file_path().c_str());
    }

}



TEST(test_msfs, download) {

    PTP::File::FileImgDownloadReq msg;
    msg.set_file_path("g0/000/000/1674392805539081_123145533718_0_1_1.dat");
    ImPdu pdu;
    pdu.SetPBMsg(&msg,CID_FileImgDownloadReq,0);
    pdu.Dump();
    CSimpleBuffer request = *new CSimpleBuffer();
    CSimpleBuffer response = *new CSimpleBuffer();
    string ip = "127.0.0.1";
    uint16_t port = 7841;
    request.Write((void *)pdu.GetBuffer(), pdu.GetLength());
    auto ret = socketSend(ip.c_str(),port,&request,&response);
    if(ret == SOCKET_CLIENT_OK){
        DEBUG_D("socketSend ret:%d",ret);
        ASSERT_EQ(response.GetLength()> 0, true);
        ImPdu *pPdu;
        pPdu = (ImPdu *)CImPdu::ReadPdu(response.GetBuffer(), response.GetLength());
        pPdu->Dump();
        ASSERT_EQ(pPdu->GetCommandId(), CID_FileImgDownloadRes);
        PTP::File::FileImgDownloadRes msg_rsp;
        msg_rsp.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength());
        DEBUG_D("error:%d",msg_rsp.error());
    }

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}