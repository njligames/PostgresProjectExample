cmake_minimum_required(VERSION 3.28)
project(PostgresProject)

set(CMAKE_CXX_STANDARD 14)


find_package(PostgreSQL REQUIRED)
include_directories(${PostgreSQL_INCLUDE_DIRS})

add_executable(${CMAKE_PROJECT_NAME} main.cpp
        Database.cpp
        Database.h)

target_link_libraries(${CMAKE_PROJECT_NAME} ${PostgreSQL_LIBRARIES})
