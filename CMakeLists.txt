cmake_minimum_required(VERSION 2.8.4)
project(nixnote2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_custom_target(nixnote2 COMMAND make debug -C ${PROJECT_SOURCE_DIR})