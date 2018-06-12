cmake_minimum_required(VERSION 2.8.4)
project(rs-nixnote2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_custom_target(rs-nixnote2 COMMAND make -C ${PROJECT_SOURCE_DIR})