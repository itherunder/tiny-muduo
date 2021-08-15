cmake_minimum_required(VERSION 2.6)

project(thread_CXX)

set(CXX_FLAGS -g -Wall)
set(CMAKE_CXX_COMPILER "g++")
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(thread_test Thread.cpp thread_test.cpp)
target_link_libraries(thread_test pthread)