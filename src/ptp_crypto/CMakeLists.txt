INCLUDE_DIRECTORIES(./)
INCLUDE_DIRECTORIES(../../include)

if(ANDROID)
    INCLUDE_DIRECTORIES(../../../boringssl/include)
    LINK_DIRECTORIES(../../../boringssl/lib)
elseif(APPLE)
    INCLUDE_DIRECTORIES(/usr/local/opt/openssl@1.1/include)
    LINK_DIRECTORIES(/usr/local/opt/openssl@1.1/lib)
elseif(UNIX)
    INCLUDE_DIRECTORIES(/usr/local/opt/openssl@1.1/include)
    LINK_DIRECTORIES(/usr/local/opt/openssl@1.1/lib)
endif()


SET(LIBRARY_OUTPUT_PATH  ../)
LINK_DIRECTORIES(../)

set(LIB_NAME_MAIN ptp_crypto)

AUX_SOURCE_DIRECTORY(. SRC_LIST)
add_library(${LIB_NAME_MAIN} STATIC ${SRC_LIST})
TARGET_LINK_LIBRARIES(${LIB_NAME_MAIN} secp256k1 crypto)
