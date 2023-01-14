cmake_minimum_required(VERSION 2.8.12)
set(TEST_NAME test_toolbox)
MESSAGE( STATUS ===>>> ${TEST_NAME} )
SET(EXECUTABLE_OUTPUT_PATH  ../../build/bin)
add_executable(${TEST_NAME}.run ${TEST_NAME}.cpp)
target_link_libraries(${TEST_NAME}.run PRIVATE gtest_main ptp_toolbox)


