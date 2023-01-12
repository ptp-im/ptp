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

SET(EXECUTABLE_OUTPUT_PATH  ../../build/bin/tests)

file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
file(COPY log4cxx.properties
        DESTINATION ${EXECUTABLE_OUTPUT_PATH})

include_directories (../../third_party/gtest/googlemock/include)
include_directories (../../third_party/gtest/googletest/include)

include_directories (../ptp_protobuf)
include_directories (../ptp_protobuf/v2)

add_executable(test_global.run test_global.cpp)
target_link_libraries(test_global.run PRIVATE gtest_main ptp_crypto ptp_global)

add_executable(test_server_msfs.run test_server_msfs.cpp)
target_link_libraries(test_server_msfs.run PRIVATE gtest_main ptp_global)

add_executable(test_server_route.run test_server_route.cpp)
target_link_libraries(test_server_route.run PRIVATE gtest_main ptp_global)

add_executable(test_server_login.run test_server_login.cpp)
target_link_libraries(test_server_login.run PRIVATE gtest_main ptp_global)

add_executable(test_server_msg.run test_server_msg.cpp)
target_link_libraries(test_server_msg.run PRIVATE gtest_main ptp_server ptp_net ptp_global ptp_wallet ptp_crypto ptp_toolbox)

add_executable(test_protobuf.run test_protobuf.cpp)
target_link_libraries(test_protobuf.run PRIVATE gtest_main ptp_protobuf ptp_global)

add_executable(test_global_SocketClient.run test_global_SocketClient.cpp)
target_link_libraries(test_global_SocketClient.run PRIVATE gtest_main ptp_global)

add_executable(test_slog.run test_slog.cpp)
target_link_libraries(test_slog.run PRIVATE gtest_main slog ptp_global)

add_executable(test_wallet.run test_wallet.cpp)
target_link_libraries(test_wallet.run PRIVATE gtest_main ptp_global ptp_wallet ptp_crypto ptp_toolbox)

add_executable(test_net.run test_net.cpp)
target_link_libraries(test_net.run PRIVATE gtest_main ptp_net ptp_global ptp_wallet ptp_crypto ptp_toolbox)

add_executable(test_net_utils.run test_net_utils.cpp)
target_link_libraries(test_net_utils.run PRIVATE gtest_main ptp_net ptp_global ptp_wallet ptp_crypto ptp_toolbox)

add_executable(test_cachepool.run test_cachepool.cpp)
target_link_libraries(test_cachepool.run PRIVATE gtest_main ptp_net ptp_global ptp_wallet ptp_crypto ptp_toolbox)

add_executable(test_dbpool.run test_dbpool.cpp)
target_link_libraries(test_dbpool.run PRIVATE gtest_main ptp_net ptp_global ptp_wallet ptp_crypto ptp_toolbox)

add_executable(test_business.run test_business.cpp)
target_link_libraries(test_business.run PRIVATE gtest_main ptp_net ptp_global ptp_wallet ptp_crypto ptp_toolbox)

add_executable(test_toolbox.run test_toolbox.cpp)
target_link_libraries(test_toolbox.run PRIVATE gtest_main ptp_toolbox)

add_executable(test_crypto.run test_crypto.cpp)
target_link_libraries(test_crypto.run PRIVATE gtest_main ptp_global ptp_crypto ptp_toolbox)

add_executable(test_secp256k1.run test_secp256k1.cpp)
target_link_libraries(test_secp256k1.run PRIVATE gtest_main ptp_global ptp_crypto crypto secp256k1)

add_executable(test_aes_gcm.run test_aes_gcm.cpp)
target_link_libraries(test_aes_gcm.run PRIVATE gtest_main ptp_global ptp_crypto crypto secp256k1)

add_executable(test_utils.run test_utils.cpp)
target_link_libraries(test_utils.run PRIVATE gtest_main ptp_global ptp_crypto ptp_protobuf crypto secp256k1)

add_executable(test_upload.run test_upload.cpp)
target_link_libraries(test_upload.run PRIVATE gtest_main ptp_global ptp_crypto ptp_protobuf crypto secp256k1)