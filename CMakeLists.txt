cmake_minimum_required(VERSION 3.20)
project(huffman C)

set(CMAKE_C_STANDARD 99)

add_executable(huffman main.c compress.c compress.h uncompress.c uncompress.h)
