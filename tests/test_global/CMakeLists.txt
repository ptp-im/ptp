cmake_minimum_required(VERSION 2.8.12)
set(TEST_NAME test_global)
MESSAGE( STATUS ===>>> ${TEST_NAME} )
add_executable(${TEST_NAME}.run ${TEST_NAME}.cpp)
target_link_libraries(${TEST_NAME}.run PRIVATE gtest_main ptp_crypto ptp_global)

add_executable(test_slog.run test_slog.cpp)
target_link_libraries(test_slog.run PRIVATE gtest_main slog ptp_global)
