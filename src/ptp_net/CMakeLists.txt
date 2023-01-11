if(NOT ANDROID)
    INCLUDE_DIRECTORIES(./)
    INCLUDE_DIRECTORIES(../)
    SET(LIBRARY_OUTPUT_PATH  ../../lib)
    LINK_DIRECTORIES(../../lib)
    set(LIB_PTP_MAIN ptp_net)
    AUX_SOURCE_DIRECTORY(. SRC_LIST)

    add_library(${LIB_PTP_MAIN} STATIC ${SRC_LIST})
    TARGET_LINK_LIBRARIES(${LIB_PTP_MAIN} ptp_global ptp_wallet ptp_crypto ptp_protobuf ptp_toolbox)
endif()

