cmake_minimum_required(VERSION 2.8.12)
cmake_policy(SET CMP0015 NEW)
if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)
project(ptp-ctl)

MESSAGE( STATUS "PROJECT_NAME: " ${PROJECT_NAME} )
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
MESSAGE( STATUS "CMAKE_INSTALL_RPATH: " "${CMAKE_INSTALL_PREFIX}/lib" )
MESSAGE( STATUS "CMAKE_CURRENT_SOURCE_DIR: " "${CMAKE_CURRENT_SOURCE_DIR}" )

file(MAKE_DIRECTORY ./build/bin)
file(MAKE_DIRECTORY ./build/lib)
file(MAKE_DIRECTORY ./build/data/msfs)
file(MAKE_DIRECTORY ./build/log)

if(APPLE)
    MESSAGE( STATUS "APPLE: ========>>" cmake ... )
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14  -g")
elseif(UNIX)
    MESSAGE( STATUS "UNIX: ========>>" cmake ... )
    ADD_DEFINITIONS( -g -W -Wall -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO -DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL -DAC_HAS_DEBUG -DLINUX_DAEMON -std=c++14 -DENCRYPT)
endif()

file(COPY ptp-server.sh DESTINATION ./build)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)

LINK_DIRECTORIES(/usr/local/lib)
INCLUDE_DIRECTORIES(/usr/local/include)

INCLUDE_DIRECTORIES(./src/)
INCLUDE_DIRECTORIES(./src/slog)
INCLUDE_DIRECTORIES(./src/ptp_protobuf)

INCLUDE_DIRECTORIES(./include)
INCLUDE_DIRECTORIES(./include/hiredis)
INCLUDE_DIRECTORIES(./include/jsoncpp)

if(APPLE)
    #mysql
    INCLUDE_DIRECTORIES(/usr/local/Cellar/mariadb/10.5.9/include/mysql)
    LINK_DIRECTORIES(/usr/local/Cellar/mariadb/10.5.9/lib)
    #openssl
    INCLUDE_DIRECTORIES(/usr/local/opt/openssl@1.1/include)
    LINK_DIRECTORIES(/usr/local/opt/openssl@1.1/lib)
else()
    INCLUDE_DIRECTORIES(/usr/include/mariadb/)
endif()

add_subdirectory(src/tools)
add_subdirectory(src/ptp_crypto)
add_subdirectory(src/ptp_toolbox)
add_subdirectory(src/ptp_net)
add_subdirectory(src/ptp_wallet)
add_subdirectory(src/ptp_global)
add_subdirectory(src/ptp_protobuf)
add_subdirectory(src/ptp_server)
add_subdirectory(src/ptp_server_business)
add_subdirectory(src/ptp_server_msg)
add_subdirectory(src/ptp_client_msg)
#add_subdirectory(src/ptp_server_msfs)
#add_subdirectory(src/ptp_server_push)
#add_subdirectory(src/ptp_server_route)
#add_subdirectory(src/ptp_server_login)
#add_subdirectory(src/ptp_server_ws)
add_subdirectory(src/slog)
add_subdirectory(third_party/gtest)
add_subdirectory(tests)
enable_testing()


