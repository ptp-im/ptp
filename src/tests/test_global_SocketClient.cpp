#include <gtest/gtest.h>
#include "ptp_global/Logger.h"
#include "ptp_global/SocketClient.h"


TEST(SocketClient, socketSend) {
    auto *request = new CSimpleBuffer();
    auto *response = new CSimpleBuffer();
    void * body = (void *) "sss";
    request->Write(body,3);
    char const* ip = "127.0.0.1";
    auto port = 7481;
    socketSend(ip,port,request,response);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
