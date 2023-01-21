#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/HeartBeatNotifyAction.h"
#include "ptp_server/actions/models/ModelOther.h"
#include "PTP.Other.pb.h"

using namespace PTP::Common;

TEST(test_Other, HeartBeatNotifyAction) {

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
