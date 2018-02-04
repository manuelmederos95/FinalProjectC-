cmake_minimum_required(VERSION 3.9)
project(ClientApp)

set(CMAKE_CXX_STANDARD 11)

add_executable(ClientApp main.cpp cmake-build-debug/TCPClient.cpp cmake-build-debug/TCPClient.h)