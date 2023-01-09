INCLUDE_DIRECTORIES(../)

INCLUDE_DIRECTORIES(../../third_party/jsoncpp)

INCLUDE_DIRECTORIES(../../third_party/slog)
LINK_DIRECTORIES(../../third_party/slog/lib)

SET(LIBRARY_OUTPUT_PATH  ../../lib)
set(LIB_NAME_MAIN ptp_business)

AUX_SOURCE_DIRECTORY(. SRC_LIST)

add_library(${LIB_NAME_MAIN} STATIC ${SRC_LIST})

TARGET_LINK_LIBRARIES(${LIB_NAME_MAIN} slog ptp_global)

