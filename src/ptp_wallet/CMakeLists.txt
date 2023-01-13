if(ANDROID)
    INCLUDE_DIRECTORIES(../../../boringssl/include)
    LINK_DIRECTORIES(../../../boringssl/lib)
    SET(LIBRARY_OUTPUT_PATH  ../../lib)
    LINK_DIRECTORIES(../../lib)
elseif()
    SET(LIBRARY_OUTPUT_PATH ../../build/lib)
    LINK_DIRECTORIES(${LIBRARY_OUTPUT_PATH})
endif()

INCLUDE_DIRECTORIES(./)
INCLUDE_DIRECTORIES(../)
INCLUDE_DIRECTORIES(../../include)

set(LIB_NAME_MAIN ptp_wallet)

AUX_SOURCE_DIRECTORY(. SRC_LIST)
AUX_SOURCE_DIRECTORY(./data/word_lists SRC_LIST)

add_library(${LIB_NAME_MAIN} STATIC ${SRC_LIST})

TARGET_LINK_LIBRARIES(${LIB_NAME_MAIN} crypto ptp_crypto ptp_toolbox)
