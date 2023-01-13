cmake_minimum_required(VERSION 2.8.12)
set(TEST_NAME test_crypto)
MESSAGE( STATUS ===>>> ${TEST_NAME} )
SET(EXECUTABLE_OUTPUT_PATH  ../../build/bin/tests)
add_executable(${TEST_NAME}.run ${TEST_NAME}.cpp)
target_link_libraries(${TEST_NAME}.run PRIVATE gtest_main ptp_global ptp_crypto ptp_toolbox)

add_executable(test_secp256k1.run test_secp256k1.cpp)
target_link_libraries(test_secp256k1.run PRIVATE gtest_main ptp_global ptp_crypto crypto secp256k1)

add_executable(test_aes_gcm.run test_aes_gcm.cpp)
target_link_libraries(test_aes_gcm.run PRIVATE gtest_main ptp_global ptp_crypto crypto secp256k1)
