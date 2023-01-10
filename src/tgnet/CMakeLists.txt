if(NOT ANDROID)
    INCLUDE_DIRECTORIES(./)

    set(LIB_NAME_MAIN tgnet)

    MESSAGE( STATUS ===>>> ${LIB_NAME_MAIN} )
    AUX_SOURCE_DIRECTORY(. SRC_LIST)

    add_library(${LIB_NAME_MAIN} STATIC
            ApiScheme.cpp
            BuffersStorage.cpp
            ByteArray.cpp
            ByteStream.cpp
            Connection.cpp
            ConnectionSession.cpp
            ConnectionsManager.cpp
            ConnectionSocket.cpp
            Datacenter.cpp
            EventObject.cpp
            FileLog.cpp
            MTProtoScheme.cpp
            NativeByteBuffer.cpp
            Request.cpp
            Timer.cpp
            TLObject.cpp
            ProxyCheckInfo.cpp
            Handshake.cpp
            Config.cpp
            AccountManager.cpp)

    TARGET_LINK_LIBRARIES(${LIB_NAME_MAIN} ptp_global ptp_wallet ptp_crypto ptp_toolbox)
endif()

