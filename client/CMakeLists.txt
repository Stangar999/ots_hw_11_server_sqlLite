cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

set(PROJECT_VERSION 1)
project(client VERSION ${PROJECT_VERSION})

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

add_executable(client_e main.cpp)

target_include_directories(client_e PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(client_e PUBLIC ${Boost_LIBRARIES})
