cmake_minimum_required(VERSION 2.8.12)
cmake_policy(SET CMP0015 NEW)
if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project(PTP)
#cmake -DIS_DOCKER=OFF
option(IS_DOCKER "IS DOCKER" OFF)

MESSAGE( STATUS "IS_DOCKER: " "${IS_DOCKER}" )

MESSAGE( STATUS "PROJECT_NAME: " ${PROJECT_NAME} )

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
MESSAGE( STATUS "CMAKE_INSTALL_RPATH: " "${CMAKE_INSTALL_PREFIX}/lib" )

file(MAKE_DIRECTORY ./build/conf)
file(MAKE_DIRECTORY ./build/bin)
file(MAKE_DIRECTORY ./build/lib)
file(MAKE_DIRECTORY ./build/log)

file(COPY ./conf/bd_server.conf
        DESTINATION ./build/conf)

if(APPLE)
    MESSAGE( STATUS "APPLE: ========>>" cmake ... )
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14  -g")
    SET(LIBRARY_PATH ./third_party/libs/mac)
elseif(UNIX)
    MESSAGE( STATUS "UNIX: ========>>" cmake ... )
    ADD_DEFINITIONS( -g -W -Wall -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO -DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL -DAC_HAS_DEBUG -DLINUX_DAEMON -std=c++11 -DENCRYPT)
    SET(LIBRARY_PATH ./third_party/libs/linux)
endif()

file(COPY ${LIBRARY_PATH}/
        DESTINATION ./build/lib)

LINK_DIRECTORIES(./build/lib)
SET(LIBRARY_OUTPUT_PATH ../../build/lib)

INCLUDE_DIRECTORIES(./src/)
INCLUDE_DIRECTORIES(./include)
INCLUDE_DIRECTORIES(./include/protobuf)
INCLUDE_DIRECTORIES(./include/redis)
INCLUDE_DIRECTORIES(./include/jsoncpp)
INCLUDE_DIRECTORIES(./include/slog)

if(APPLE)
    #mysql
    INCLUDE_DIRECTORIES(/usr/local/Cellar/mariadb/10.5.9/include/mysql)
    LINK_DIRECTORIES(/usr/local/Cellar/mariadb/10.5.9/lib)
    #openssl
    INCLUDE_DIRECTORIES(/usr/local/opt/openssl@1.1/include)
    LINK_DIRECTORIES(/usr/local/opt/openssl@1.1/lib)
else()

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
#add_subdirectory(src/ptp_server_msg)
#add_subdirectory(src/ptp_server_msfs)
#add_subdirectory(src/ptp_server_push)
#add_subdirectory(src/ptp_server_route)
#add_subdirectory(src/ptp_server_login)
#add_subdirectory(src/ptp_server_ws)

add_subdirectory(src/tests)
add_subdirectory(third_party/gtest)
enable_testing()


