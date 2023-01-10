cmake_minimum_required(VERSION 2.8.12)
cmake_policy(SET CMP0015 NEW)
if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project(PTP)

MESSAGE( STATUS "PROJECT_NAME: " ${PROJECT_NAME} )

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
MESSAGE( STATUS "CMAKE_INSTALL_RPATH: " "${CMAKE_INSTALL_PREFIX}/lib" )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14  -g")

INCLUDE_DIRECTORIES(./src/)
LINK_DIRECTORIES(./lib)
SET(LIBRARY_OUTPUT_PATH ./lib)

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
    INCLUDE_DIRECTORIES(/usr/include/mysql)
    LINK_DIRECTORIES(/usr/lib64/mysql)
    INCLUDE_DIRECTORIES(/usr/local/opt/openssl@1.1/include)
    LINK_DIRECTORIES(/usr/local/opt/openssl@1.1/lib)
endif()

INCLUDE_DIRECTORIES(./include/protobuf)
INCLUDE_DIRECTORIES(./include/redis)
INCLUDE_DIRECTORIES(./include)

add_subdirectory(src/ptp_crypto)
add_subdirectory(src/ptp_toolbox)
add_subdirectory(src/ptp_net)
add_subdirectory(src/ptp_wallet)
add_subdirectory(src/ptp_global)
add_subdirectory(src/ptp_protobuf)
add_subdirectory(src/ptp_business)
add_subdirectory(src/ptp_servers)
add_subdirectory(src/ptp_server_msfs)
add_subdirectory(src/ptp_server_push)
add_subdirectory(src/ptp_server_route)
add_subdirectory(src/ptp_server_login)
add_subdirectory(src/ptp_server_ws)
add_subdirectory(src/ptp_server_business)
#add_subdirectory(src/tgnet)
add_subdirectory(third_party/slog)
add_subdirectory(src/tests)
enable_testing()


