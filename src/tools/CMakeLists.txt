cmake_minimum_required(VERSION 2.8.12)
set(LIB_NAME_MAIN tools)

project(${LIB_NAME_MAIN})

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)

file(COPY ptp.sh
        DESTINATION ../../build/bin)

file(COPY ptp_server.sh
        DESTINATION ../../build/bin)

endif(COMMAND cmake_policy)
MESSAGE( STATUS ===>>> ${LIB_NAME_MAIN} )
SET(EXECUTABLE_OUTPUT_PATH  ../../build/bin)
file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
ADD_EXECUTABLE(daeml daeml.cpp)
