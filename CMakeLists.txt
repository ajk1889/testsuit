cmake_minimum_required(VERSION 3.16)
project(testsuit)

set(GCC_PTHREAD_FLAG "-pthread")
set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_PTHREAD_FLAG}")
set(CMAKE_CXX_STANDARD 14)
include_directories(${CURL_INCLUDE_DIR})

add_executable(
        testsuit main.cpp
        implementations/networking/Response.cpp
        implementations/networking/Response.h
        implementations/networking/HttpRequest.cpp
        implementations/networking/HttpRequest.h
        server/Server.cpp server/Server.h
        implementations/Generator.cpp
        implementations/Generator.h
        implementations/Pair.h
        implementations/networking/ServerSocket.cpp
        implementations/networking/ServerSocket.h
        implementations/networking/Socket.cpp
        implementations/networking/Socket.h
        server/ServerParams.h
        implementations/constants.h
        implementations/helpers.h
        implementations/helper.cpp
        implementations/ArrayJoiner.h
)

target_link_libraries(testsuit ${CURL_LIBRARIES})