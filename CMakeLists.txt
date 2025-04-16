cmake_minimum_required(VERSION 3.28)
project(PostgresProject VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 14)

option(BUILD_TESTS "Build the tests" OFF)
option(BUILD_EXECUTABLE "Build the tests" OFF)

find_package(PostgreSQL REQUIRED)

add_library(MosaifyDB STATIC
        MosaifyDatabase.cpp
        MosaifyDatabase.h
        IImageData.h)

target_link_libraries(MosaifyDB ${PostgreSQL_LIBRARIES})

target_include_directories(MosaifyDB
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR} ${PostgreSQL_INCLUDE_DIRS}>
        $<INSTALL_INTERFACE:include>
)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Executable

if(BUILD_EXECUTABLE)
    add_executable(${CMAKE_PROJECT_NAME} main.cpp )
    target_link_libraries(${CMAKE_PROJECT_NAME} MosaifyDB)
endif()
