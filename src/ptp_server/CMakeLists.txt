cmake_minimum_required(VERSION 2.8.12)
set(LIB_NAME_MAIN ptp_server)

project(${LIB_NAME_MAIN})

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

MESSAGE( STATUS ===>>> ${LIB_NAME_MAIN} )

INCLUDE_DIRECTORIES(./)
INCLUDE_DIRECTORIES(../ptp_protobuf/)
SET(LIBRARY_OUTPUT_PATH ../../build/lib)
SET(EXECUTABLE_OUTPUT_PATH  ../../build/bin/${LIB_NAME_MAIN})
file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
file(COPY log4cxx.properties
        DESTINATION ${EXECUTABLE_OUTPUT_PATH})

set(COMMON_LIBS pthread mysqlclient_r curl hiredis slog crypto secp256k1 ptp_global ptp_crypto ptp_protobuf ptp_wallet)
AUX_SOURCE_DIRECTORY(. SRC_LIST)
AUX_SOURCE_DIRECTORY(./actions SRC_LIST)
AUX_SOURCE_DIRECTORY(./models SRC_LIST)
AUX_SOURCE_DIRECTORY(./services SRC_LIST)

add_library(${LIB_NAME_MAIN} STATIC ${SRC_LIST})
TARGET_LINK_LIBRARIES(${LIB_NAME_MAIN} ${COMMON_LIBS})

ADD_EXECUTABLE(${LIB_NAME_MAIN}.app main.cpp)
TARGET_LINK_LIBRARIES(${LIB_NAME_MAIN}.app ${LIB_NAME_MAIN})
