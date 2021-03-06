cmake_minimum_required(VERSION 3.13)
project(testsuit)

set(GCC_PTHREAD_FLAG "-pthread")
set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_PTHREAD_FLAG}")
set(CMAKE_CXX_STANDARD 17)
include_directories(${CURL_INCLUDE_DIR})

add_executable(
        testsuit main.cpp
        implementations/networking/http/response/HttpResponse.h
        implementations/networking/http/request/HttpRequest.cpp
        implementations/networking/http/request/HttpRequest.h
        server/Server.cpp server/Server.h
        implementations/networking/ServerSocket.cpp
        implementations/networking/ServerSocket.h
        implementations/networking/Socket.cpp
        implementations/networking/Socket.h
        server/ServerParams.h
        implementations/constants.h
        implementations/helpers.h
        implementations/helper.cpp
        implementations/utils/ArrayJoiner.h
        implementations/file/FileOrString.h
        implementations/networking/http/ContentDisposition.h
        implementations/file/FileOrString.cpp
        implementations/Constants.cpp
        implementations/utils/Process.cpp
        implementations/utils/Process.h
        implementations/networking/http/response/StringResponse.cpp
        implementations/networking/http/response/StringResponse.h
        implementations/networking/http/response/FileResponse.cpp
        implementations/networking/http/response/FileResponse.h
        implementations/file/StreamDescriptor.cpp
        implementations/file/StreamDescriptor.h
        implementations/networking/http/response/DescriptorResponse.cpp
        implementations/networking/http/response/DescriptorResponse.h
        server/ServerParams.cpp
        implementations/utils/UrlMapObject.h
        test.cpp
)

target_link_libraries(testsuit ${CURL_LIBRARIES})