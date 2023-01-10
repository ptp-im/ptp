cmake_minimum_required(VERSION 3.2.2)

option(BUILD_GMOCK "Builds the googlemock subproject" OFF)
option(INSTALL_GTEST "Enable installation of googletest. (Projects embedding googletest may want to turn this OFF.)" OFF)

MESSAGE( STATUS ===>>> tests )
include(FetchContent)
if(APPLE)
    MESSAGE( STATUS "GTEST release-1.11.0 APPLE" )
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY /Users/jack/data/git/gtest.git
            GIT_TAG        release-1.11.0
    )
else()
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        release-1.11.0
    )
    MESSAGE( STATUS "GTEST release-1.11.0 LINUX" )
endif()

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

if (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
endif (APPLE)

ADD_DEFINITIONS(-DGTEST_USE_OWN_TR1_TUPLE=1 )

SET(EXECUTABLE_OUTPUT_PATH  ../../bin/tests)

file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
file(COPY log4cxx.properties
        DESTINATION ${EXECUTABLE_OUTPUT_PATH})


include_directories (../ptp_protobuf/v2)

add_executable(test_global.run test_global.cpp)
target_link_libraries(test_global.run PRIVATE gtest_main ptp_global)

add_executable(test_server_msfs.run test_server_msfs.cpp)
target_link_libraries(test_server_msfs.run PRIVATE gtest_main ptp_servers ptp_global)

add_executable(test_server_route.run test_server_route.cpp)
target_link_libraries(test_server_route.run PRIVATE gtest_main ptp_servers ptp_global)

add_executable(test_server_login.run test_server_login.cpp)
target_link_libraries(test_server_login.run PRIVATE gtest_main ptp_servers ptp_global)

add_executable(test_protobuf.run test_protobuf.cpp)
target_link_libraries(test_protobuf.run PRIVATE gtest_main ptp_protobuf ptp_global)

add_executable(test_global_SocketClient.run test_global_SocketClient.cpp)
target_link_libraries(test_global_SocketClient.run PRIVATE gtest_main ptp_global)

add_executable(test_slog.run test_slog.cpp)
target_link_libraries(test_slog.run PRIVATE gtest_main slog ptp_global)

add_executable(test_wallet.exe test_wallet.cpp)
target_link_libraries(test_wallet.exe PRIVATE gtest_main ptp_global ptp_wallet ptp_crypto ptp_toolbox)

add_executable(test_toolbox.exe test_toolbox.cpp)
target_link_libraries(test_toolbox.exe PRIVATE gtest_main ptp_toolbox)

add_executable(test_crypto.exe test_crypto.cpp)
target_link_libraries(test_crypto.exe PRIVATE gtest_main ptp_crypto ptp_toolbox)
#
add_executable(test_net.exe test_net.cpp)
target_link_libraries(test_net.exe PRIVATE gtest_main crypto ptp_net ptp_wallet ptp_crypto ptp_toolbox)