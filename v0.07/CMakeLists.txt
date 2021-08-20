cmake_minimum_required(VERSION 2.6)

project(epoll_CXX)

set(CXX_FLAGS -g -Wall)
set(CMAKE_CXX_COMPILER "g++")
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(srv main.cc TcpServer.cc Channel.cc Acceptor.cc TcpConnection.cc Epoll.cc EventLoop.cc EchoServer.cc)
add_executable(cli cli.cc)
