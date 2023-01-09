INCLUDE_DIRECTORIES(../)

INCLUDE_DIRECTORIES(../../third_party/jsoncpp)

if(NOT ANDROID)
    INCLUDE_DIRECTORIES(../../third_party/slog)
    LINK_DIRECTORIES(../../third_party/slog/lib)
endif()

SET(LIBRARY_OUTPUT_PATH  ../../lib)
set(LIB_NAME_MAIN ptp_global)
MESSAGE( STATUS ===>>> ${LIB_NAME_MAIN} )
AUX_SOURCE_DIRECTORY(. SRC_LIST)
AUX_SOURCE_DIRECTORY(../../third_party/jsoncpp SRC_LIST)

add_library(${LIB_NAME_MAIN} STATIC ${SRC_LIST})

if(NOT ANDROID)
    TARGET_LINK_LIBRARIES(${LIB_NAME_MAIN} slog)
else()
    TARGET_LINK_LIBRARIES(${LIB_NAME_MAIN})
endif()

