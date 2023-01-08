cmake_minimum_required(VERSION 2.8.12)
cmake_policy(SET CMP0015 NEW)
if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project(PTP)

# Check platforms
if (CMAKE_HOST_WIN32)
    set(WINDOWS 1)
elseif(CMAKE_HOST_APPLE)
    set(MACOS 1)
elseif(CMAKE_HOST_UNIX)
    set(LINUX 1)
endif()

MESSAGE( STATUS "PROJECT_NAME: " ${PROJECT_NAME} )

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
MESSAGE( STATUS "CMAKE_INSTALL_RPATH: " "${CMAKE_INSTALL_PREFIX}/lib" )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14  -g")
SET(EXECUTABLE_OUTPUT_PATH  ./)

LINK_DIRECTORIES(./lib ../boringssl/lib/)
INCLUDE_DIRECTORIES(../boringssl/include)
add_subdirectory(src/ptp_common)
add_subdirectory(src/ptp_toolbox)
add_subdirectory(src/ptp_crypto)
add_subdirectory(src/ptp_wallet)
add_subdirectory(src/ptp_net)
add_subdirectory(src/tests)
enable_testing()


