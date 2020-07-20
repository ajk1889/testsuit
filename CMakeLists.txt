cmake_minimum_required(VERSION 3.16)
project(testsuit)

set(CMAKE_CXX_STANDARD 14)

add_executable(testsuit main.cpp Response.cpp Response.h Request.cpp Request.h Server.cpp Server.h implementations/Generator.cpp implementations/Generator.h Pair.h implementations/ServerSocket.cpp implementations/ServerSocket.h implementations/Socket.cpp implementations/Socket.h)