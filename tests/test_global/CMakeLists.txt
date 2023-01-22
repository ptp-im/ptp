cmake_minimum_required(VERSION 2.8.12)
set(TEST_NAME test_global)
MESSAGE( STATUS ===>>> ${TEST_NAME} )

add_executable(${TEST_NAME}.run ${TEST_NAME}.cpp)
target_link_libraries(${TEST_NAME}.run PRIVATE gtest_main ptp_crypto ptp_global)

add_executable(test_log.run test_log.cpp)
target_link_libraries(test_log.run PRIVATE gtest_main slog ptp_global)

add_executable(test_socket_client.run test_socket_client.cpp)
target_link_libraries(test_socket_client.run PRIVATE gtest_main test_init ptp_global ptp_protobuf)

add_executable(test_curl.run test_curl.cpp)
target_link_libraries(test_curl.run PRIVATE gtest_main test_init ptp_curl ptp_global ptp_protobuf)

add_executable(test_uri.run test_uri.cpp)
target_link_libraries(test_uri.run PRIVATE gtest_main test_init ptp_global ptp_protobuf)
