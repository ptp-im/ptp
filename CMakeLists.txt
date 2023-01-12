cmake_minimum_required(VERSION 2.8.12)
cmake_policy(SET CMP0015 NEW)
if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project(PTP)

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
    file(COPY ${LIBRARY_PATH}/libslog.dylib
            DESTINATION ./build/lib)
    file(COPY ${LIBRARY_PATH}/liblog4cxx.10.0.0.dylib
            DESTINATION ./build/lib)
    file(COPY ${LIBRARY_PATH}/liblog4cxx.10.dylib
            DESTINATION ./build/lib)
    file(COPY ${LIBRARY_PATH}/liblog4cxx.a
            DESTINATION ./build/lib)
    file(COPY ${LIBRARY_PATH}/liblog4cxx.dylib
            DESTINATION ./build/lib)
    file(COPY ${LIBRARY_PATH}/liblog4cxx.la
            DESTINATION ./build/lib)
elseif(UNIX)
    MESSAGE( STATUS "UNIX: ========>>" cmake ... )

    ADD_DEFINITIONS( -g -W -Wall -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO -DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL -DAC_HAS_DEBUG -DLINUX_DAEMON -std=c++11 -DENCRYPT)
    SET(LIBRARY_PATH ./third_party/libs/linux)
    file(COPY ${LIBRARY_PATH}/libslog.so
            DESTINATION ./build/lib)
endif()

file(COPY ${LIBRARY_PATH}/libprotobuf-lite.a
        DESTINATION ./build/lib)
file(COPY ${LIBRARY_PATH}/libhiredis.a
        DESTINATION ./build/lib)
file(COPY ${LIBRARY_PATH}/libsecp256k1.a
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
    INCLUDE_DIRECTORIES(/usr/include/mysql)
    LINK_DIRECTORIES(/usr/lib64/mysql)
    INCLUDE_DIRECTORIES(/usr/local/opt/openssl@1.1/include)
    LINK_DIRECTORIES(/usr/local/opt/openssl@1.1/lib)
endif()

add_subdirectory(src/tools)
add_subdirectory(src/ptp_crypto)
add_subdirectory(src/ptp_toolbox)
#add_subdirectory(src/ptp_net)
add_subdirectory(src/ptp_wallet)
add_subdirectory(src/ptp_global)
add_subdirectory(src/ptp_protobuf)
add_subdirectory(src/ptp_server)
add_subdirectory(src/ptp_server_business)
add_subdirectory(src/ptp_server_msg)
#add_subdirectory(src/ptp_server_msfs)
#add_subdirectory(src/ptp_server_push)
#add_subdirectory(src/ptp_server_route)
#add_subdirectory(src/ptp_server_login)
#add_subdirectory(src/ptp_server_ws)
#add_subdirectory(src/ptp_server_business)
#add_subdirectory(third_party/slog)

if(APPLE)
    add_subdirectory(src/tests)
    enable_testing()
endif()



