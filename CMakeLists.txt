cmake_minimum_required(VERSION 3.28)
project(8zip)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")

add_executable(8zip huffman/huffman.cpp
)
