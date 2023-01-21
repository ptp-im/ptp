cmake_minimum_required(VERSION 2.8.12)
project(PTP_test)

option(BUILD_GMOCK "Builds the googlemock subproject" OFF)
option(INSTALL_GTEST "Enable installation of googletest. (Projects embedding googletest may want to turn this OFF.)" OFF)

MESSAGE( STATUS ===>>> tests )

#include(FetchContent)
#if(APPLE)
#    MESSAGE( STATUS "GTEST release-1.11.0 APPLE" )
#    FetchContent_Declare(
#            googletest
#            GIT_REPOSITORY /Users/jack/data/git/gtest.git
#            GIT_TAG        release-1.11.0
#    )
#else()
#    MESSAGE( STATUS "GTEST release-1.11.0 LINUX" )
#    FetchContent_Declare(
#            googletest
#            GIT_REPOSITORY https://github.com/google/googletest.git
#            GIT_TAG        release-1.11.0
#    )
#endif()
#FetchContent_GetProperties(googletest)
#if(NOT googletest_POPULATED)
#    FetchContent_Populate(googletest)
#    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
#endif()

if (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
endif (APPLE)

ADD_DEFINITIONS(-DGTEST_USE_OWN_TR1_TUPLE=1 )

file(COPY log4cxx.nostdout.properties DESTINATION ${EXECUTABLE_OUTPUT_PATH})

include_directories (../third_party/gtest/googlemock/include)
include_directories (../third_party/gtest/googletest/include)

include_directories (../src/ptp_protobuf)
include_directories (../src/ptp_protobuf/v2)
include_directories (./)

add_library(test_init STATIC test_init.cpp)
TARGET_LINK_LIBRARIES(test_init ptp_server ptp_global)

add_subdirectory(test_blank)
add_subdirectory(test_crypto)
add_subdirectory(test_global)
add_subdirectory(test_net)
add_subdirectory(test_protobuf)
add_subdirectory(test_server)
add_subdirectory(test_toolbox)
add_subdirectory(test_wallet)
add_subdirectory(test_slog)