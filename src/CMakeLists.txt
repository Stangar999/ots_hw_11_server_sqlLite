cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

if (MINGW)
    set(BOOST_ROOT "C:/Program_/Boost/boost_1.81.0_new_MGW")
    set(Boost_COMPILER mgw8)
endif()

find_package(Boost REQUIRED system)
find_package(Threads REQUIRED)

include(FetchContent)
FetchContent_Declare(sqlite3 URL "https://www.sqlite.org/snapshot/sqlite-snapshot-202205041843.tar.gz")
FetchContent_MakeAvailable(sqlite3)
if (NOT sqlite3_POPULATED)
  FetchContent_Populate(sqlite3)
endif()

add_executable(join_server_e main.cpp server.h request_handler.h request_handler.cpp "${sqlite3_SOURCE_DIR}/sqlite3.c")

target_link_libraries(join_server_e PRIVATE ${Boost_LIBRARIES} Threads::Threads )

target_include_directories(join_server_e PUBLIC "${sqlite3_SOURCE_DIR}")
