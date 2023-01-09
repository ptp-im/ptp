SET(LIBRARY_OUTPUT_PATH  ../../lib)
set(LIB_NAME_MAIN ptp_protobuf)
MESSAGE( STATUS ===>>> ${LIB_NAME_MAIN} )

if(APPLE)
    #lib:protobuf
    LINK_DIRECTORIES(../../third_party/lib/mac)
else()
    LINK_DIRECTORIES(../../third_party/lib)
endif()

INCLUDE_DIRECTORIES(../../third_party/protobuf)
AUX_SOURCE_DIRECTORY(./ SRC_LIST)

add_library(${LIB_NAME_MAIN} STATIC ${SRC_LIST})

TARGET_LINK_LIBRARIES(${LIB_NAME_MAIN} protobuf-lite)
