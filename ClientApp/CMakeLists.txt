cmake_minimum_required(VERSION 3.9)
project(ClientApp)

set(CMAKE_CXX_STANDARD 11)


SET(CMAKE_CXX_FLAGS -pthread)

add_executable(ClientApp main.cpp cmake-build-debug/TCPClient.cpp cmake-build-debug/TCPClient.h cmake-build-debug/clavier_non_bloquant.cpp cmake-build-debug/clavier_non_bloquant.h)