cmake_minimum_required(VERSION 2.8.12)
set(TEST_NAME test_slog)
MESSAGE( STATUS ===>>> ${TEST_NAME} )

add_executable(${TEST_NAME}.run main.cpp)
target_link_libraries(${TEST_NAME}.run PRIVATE gtest_main slog log4cxx)