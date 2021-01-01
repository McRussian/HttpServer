cmake_minimum_required(VERSION 2.8)
project(HtppServer)

set(CMAKE_CXX_STANDARD 11)

add_executable(final main.cpp arg_parser.h)
target_link_libraries(final pthread)
