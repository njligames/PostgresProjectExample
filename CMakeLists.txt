cmake_minimum_required(VERSION 3.28)
project(PostgresProject)

set(CMAKE_CXX_STANDARD 14)

# Add the source directory
add_subdirectory(MosaifyDatabase)

add_executable(${CMAKE_PROJECT_NAME} main.cpp )

target_link_libraries(${CMAKE_PROJECT_NAME} MosaifyDB)
