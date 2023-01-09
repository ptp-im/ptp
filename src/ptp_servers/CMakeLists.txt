SET(LIBRARY_OUTPUT_PATH  ../../lib)
set(LIB_NAME_MAIN ptp_servers)

MESSAGE( STATUS ===>>> ${LIB_NAME_MAIN} )

AUX_SOURCE_DIRECTORY(. SRC_LIST)
AUX_SOURCE_DIRECTORY(./route SRC_LIST)
AUX_SOURCE_DIRECTORY(./msfs SRC_LIST)
AUX_SOURCE_DIRECTORY(./login SRC_LIST)

add_library(${LIB_NAME_MAIN} STATIC ${SRC_LIST})

INCLUDE_DIRECTORIES(../)

INCLUDE_DIRECTORIES(../../third_party/jsoncpp)

INCLUDE_DIRECTORIES(../../third_party/slog)
LINK_DIRECTORIES(../../third_party/slog/lib)

if(APPLE)
    #lib:redis secp256k1 protobuf
    LINK_DIRECTORIES(../../third_party/lib/mac)
    #mysql
    INCLUDE_DIRECTORIES(/usr/local/Cellar/mariadb/10.5.9/include/mysql)
    LINK_DIRECTORIES(/usr/local/Cellar/mariadb/10.5.9/lib)

    #openssl
    INCLUDE_DIRECTORIES(/usr/local/opt/openssl@1.1/include)
    LINK_DIRECTORIES(/usr/local/opt/openssl@1.1/lib)
else()
    LINK_DIRECTORIES(../../third_party/lib)
    INCLUDE_DIRECTORIES(/usr/include/mysql)
    LINK_DIRECTORIES(/usr/lib64/mysql)
    INCLUDE_DIRECTORIES(/usr/local/opt/openssl@1.1/include)
    LINK_DIRECTORIES(/usr/local/opt/openssl@1.1/lib)
endif()

INCLUDE_DIRECTORIES(../../third_party/protobuf)
INCLUDE_DIRECTORIES(../../third_party/redis)
INCLUDE_DIRECTORIES(../../third_party/secp256k1/include)

set(COMMON_LIBS pthread mysqlclient_r hiredis curl slog crypto secp256k1)

TARGET_LINK_LIBRARIES(${LIB_NAME_MAIN} ${COMMON_LIBS} ptp_global ptp_protobuf)
