INCLUDE_DIRECTORIES(./)
include_directories (../../include)
include_directories (../../../boringssl/include)
SET(LIBRARY_OUTPUT_PATH  ../)
LINK_DIRECTORIES(../)
set(LIB_NAME_MAIN ptp_wallet)

AUX_SOURCE_DIRECTORY(. SRC_LIST)
AUX_SOURCE_DIRECTORY(./data/word_lists SRC_LIST)
add_library(${LIB_NAME_MAIN} STATIC ${SRC_LIST})

TARGET_LINK_LIBRARIES(${LIB_NAME_MAIN} crypto ptp_crypto ptp_toolbox)
