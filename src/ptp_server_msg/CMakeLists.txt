cmake_minimum_required(VERSION 2.8.12)
set(LIB_NAME_MAIN ptp_server_msg)

project(${LIB_NAME_MAIN})

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
MESSAGE( STATUS ===>>> ${LIB_NAME_MAIN} )

INCLUDE_DIRECTORIES(./)
INCLUDE_DIRECTORIES(../ptp_protobuf/)
SET(EXECUTABLE_OUTPUT_PATH  ../../build/bin/${LIB_NAME_MAIN})

file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

file(COPY log4cxx.properties
        DESTINATION ${EXECUTABLE_OUTPUT_PATH})

ADD_EXECUTABLE(${LIB_NAME_MAIN}.app main.cpp)
TARGET_LINK_LIBRARIES(${LIB_NAME_MAIN}.app ptp_server)
