cmake_minimum_required(VERSION 3.2.2)

option(BUILD_GMOCK "Builds the googlemock subproject" OFF)
option(INSTALL_GTEST "Enable installation of googletest. (Projects embedding googletest may want to turn this OFF.)" OFF)

include(FetchContent)

if(MACOS)
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

SET(EXECUTABLE_OUTPUT_PATH  ./)

include_directories (../)
LINK_DIRECTORIES(../)

add_executable(test_common.exe test_common.cpp)
target_link_libraries(test_common.exe PRIVATE gtest_main)
#
#add_executable(test_common.exe test_common.cpp)
#target_link_libraries(test_common.exe PRIVATE gtest_main ptp_common)
#
#add_executable(test_wallet.exe test_wallet.cpp)
#target_link_libraries(test_wallet.exe PRIVATE gtest_main ptp_common ptp_wallet ptp_crypto ptp_toolbox)
#
#add_executable(test_toolbox.exe test_toolbox.cpp)
#target_link_libraries(test_toolbox.exe PRIVATE gtest_main ptp_toolbox)
#
#add_executable(test_crypto.exe test_crypto.cpp)
#target_link_libraries(test_crypto.exe PRIVATE gtest_main ptp_crypto ptp_toolbox)
#
#add_executable(test_net.exe test_net.cpp)
#target_link_libraries(test_net.exe PRIVATE gtest_main crypto ptp_net ptp_wallet ptp_crypto ptp_toolbox)