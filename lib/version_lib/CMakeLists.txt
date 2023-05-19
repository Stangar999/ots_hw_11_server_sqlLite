cmake_minimum_required(VERSION 3.22)

add_library(version_l version.cpp
version.h)

target_include_directories(version_l PRIVATE "${CMAKE_BINARY_DIR}")
