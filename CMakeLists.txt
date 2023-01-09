cmake_minimum_required(VERSION 2.8.12)
cmake_policy(SET CMP0015 NEW)
if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project(PTP)

MESSAGE( STATUS "PROJECT_NAME: " ${PROJECT_NAME} )

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
MESSAGE( STATUS "CMAKE_INSTALL_RPATH: " "${CMAKE_INSTALL_PREFIX}/lib" )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14  -g")

#add_subdirectory(src/ptp_common)
#add_subdirectory(src/ptp_toolbox)
#add_subdirectory(src/ptp_crypto)
#add_subdirectory(src/ptp_wallet)
#add_subdirectory(src/ptp_net)
add_subdirectory(src/ptp_global)
add_subdirectory(src/ptp_protobuf)
add_subdirectory(src/ptp_servers)
add_subdirectory(src/ptp_server_msfs)
add_subdirectory(src/ptp_server_route)
add_subdirectory(src/ptp_server_login)
add_subdirectory(third_party/slog)
add_subdirectory(src/tests)
enable_testing()


