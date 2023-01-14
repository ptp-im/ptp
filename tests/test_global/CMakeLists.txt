cmake_minimum_required(VERSION 2.8.12)
set(TEST_NAME test_global)
MESSAGE( STATUS ===>>> ${TEST_NAME} )
SET(EXECUTABLE_OUTPUT_PATH  ../../build/bin)
add_executable(${TEST_NAME}.run ${TEST_NAME}.cpp)
target_link_libraries(${TEST_NAME}.run PRIVATE gtest_main ptp_crypto ptp_global)

add_executable(test_log.run test_log.cpp)
target_link_libraries(test_log.run PRIVATE gtest_main slog ptp_global)
