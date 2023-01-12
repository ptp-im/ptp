cmake_minimum_required(VERSION 2.8.12)
set(TEST_NAME test_server)
MESSAGE( STATUS ===>>> ${TEST_NAME} )

add_executable(test_cachepool.run test_cachepool.cpp)
target_link_libraries(test_cachepool.run PRIVATE gtest_main ptp_net ptp_global ptp_wallet ptp_crypto ptp_toolbox)

add_executable(test_dbpool.run test_dbpool.cpp)
target_link_libraries(test_dbpool.run PRIVATE gtest_main ptp_net ptp_global ptp_wallet ptp_crypto ptp_toolbox)

add_executable(test_utils.run test_utils.cpp)
target_link_libraries(test_utils.run PRIVATE gtest_main ptp_global ptp_crypto ptp_protobuf crypto secp256k1)

add_executable(test_upload.run test_upload.cpp)
target_link_libraries(test_upload.run PRIVATE gtest_main ptp_global ptp_crypto ptp_protobuf crypto secp256k1)

