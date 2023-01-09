INCLUDE_DIRECTORIES(../)
LINK_DIRECTORIES(../../lib)

SET(EXECUTABLE_OUTPUT_PATH  ./)

set(LIB_NAME_MAIN ptp_server_route)
MESSAGE( STATUS ===>>> ${LIB_NAME_MAIN} )

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

set(COMMON_LIBS pthread slog)

ADD_EXECUTABLE(${LIB_NAME_MAIN} main.cpp)
TARGET_LINK_LIBRARIES(${LIB_NAME_MAIN} ptp_servers ${COMMON_LIBS})
