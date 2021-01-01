cmake_minimum_required(VERSION 2.8)
project(http10server)

set(CMAKE_CXX_STANDARD 17)

add_executable(final main.cpp arg_parser.h)
target_link_libraries(final pthread)
