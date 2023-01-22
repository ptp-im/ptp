#include <gtest/gtest.h>
#include <array>
#include "ptp_global/SocketClient.h"
#include "ptp_global/Helpers.h"
#include "ptp_global/Logger.h"
#include "ptp_protobuf/PTP.File.pb.h"
#include "ptp_protobuf/ImPdu.h"

using namespace std;

TEST(test_socket_client, socketSend) {
    CSimpleBuffer request = *new CSimpleBuffer();
    CSimpleBuffer response = *new CSimpleBuffer();
    string ip = "127.0.0.1";
    uint16_t port = 7881;
    char const* pduBytes = "name";
    request.Write((void *)pduBytes, strlen(pduBytes));
    auto ret = socketSend(ip.c_str(),port,&request,&response);
    DEBUG_D("socketSend ret:%d",ret);
}
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}