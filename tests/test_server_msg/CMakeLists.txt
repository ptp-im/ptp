cmake_minimum_required(VERSION 2.8.12)
set(TEST_NAME test_server_msg)
MESSAGE( STATUS ===>>> ${TEST_NAME} )


add_executable(${TEST_NAME}.run ${TEST_NAME}.cpp)
target_link_libraries(${TEST_NAME}.run PRIVATE gtest_main ptp_server ptp_net ptp_global ptp_wallet ptp_crypto ptp_toolbox)


