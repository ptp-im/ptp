cmake_minimum_required(VERSION 2.8.12)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
set(LIB_NAME_MAIN ptp_server_push)
project(${LIB_NAME_MAIN})
MESSAGE( STATUS ===>>> ${LIB_NAME_MAIN} )

AUX_SOURCE_DIRECTORY(./ SRC_LIST)
AUX_SOURCE_DIRECTORY(./socket SRC_LIST)

ADD_EXECUTABLE(${LIB_NAME_MAIN} ${SRC_LIST})
TARGET_LINK_LIBRARIES(${LIB_NAME_MAIN} pthread protobuf-lite ssl slog ptp_server)
