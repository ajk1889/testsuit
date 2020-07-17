cmake_minimum_required(VERSION 3.16)
project(testsuit)

set(CMAKE_CXX_STANDARD 14)

add_executable(testsuit main.cpp Response.cpp Response.h Request.cpp Request.h Server.cpp Server.h Generator.cpp Generator.h Pair.h)