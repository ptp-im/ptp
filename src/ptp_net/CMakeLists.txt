if(NOT ANDROID)
    INCLUDE_DIRECTORIES(./)
    INCLUDE_DIRECTORIES(../)
    include_directories (../../include)
    SET(LIBRARY_OUTPUT_PATH  ../)
    LINK_DIRECTORIES(../)
    set(LIB_PTP_MAIN ptp_net)
    AUX_SOURCE_DIRECTORY(. SRC_LIST)

    add_library(${LIB_PTP_MAIN} STATIC ${SRC_LIST})
    TARGET_LINK_LIBRARIES(${LIB_PTP_MAIN} ptp_common ptp_wallet ptp_crypto ptp_toolbox)
endif()

