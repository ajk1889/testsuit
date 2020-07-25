cmake_minimum_required(VERSION 3.16)
project(testsuit)

set(GCC_COVERAGE_COMPILE_FLAGS "-pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
set(CMAKE_CXX_STANDARD 14)

add_executable(testsuit main.cpp wrappers/Response.cpp wrappers/Response.h wrappers/Request.cpp wrappers/Request.h server/Server.cpp server/Server.h implementations/Generator.cpp implementations/Generator.h implementations/Pair.h implementations/networking/ServerSocket.cpp implementations/networking/ServerSocket.h implementations/networking/Socket.cpp implementations/networking/Socket.h server/ServerParams.h implementations/constants.h implementations/helpers.h implementations/helper.cpp)