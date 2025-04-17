cmake_minimum_required(VERSION 3.28)
project(PostgresProject VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 14)

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR} CACHE PATH "My Comment" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

option(BUILD_TESTS "Build the tests" OFF)
option(BUILD_EXECUTABLE "Build the tests" OFF)

find_package(PostgreSQL REQUIRED)

add_library(MosaifyDB STATIC
        MosaifyDatabase.cpp
        )

target_link_libraries(MosaifyDB ${PostgreSQL_LIBRARIES})

target_include_directories(MosaifyDB
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR} ${PostgreSQL_INCLUDE_DIRS}>
        $<INSTALL_INTERFACE:include>
)

set(MOSAIFY_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Executable

if(BUILD_EXECUTABLE)
    add_executable(${CMAKE_PROJECT_NAME} main.cpp )
    target_link_libraries(${CMAKE_PROJECT_NAME} MosaifyDB)

    install(TARGETS ${CMAKE_PROJECT_NAME}
            ARCHIVE DESTINATION MosaifyDatabase/lib
            LIBRARY DESTINATION MosaifyDatabase/lib
            RUNTIME DESTINATION MosaifyDatabase/bin
    )
endif()

install(FILES
        ${CMAKE_SOURCE_DIR}/MosaifyDatabase/MosaifyDatabase.h
        ${CMAKE_SOURCE_DIR}/MosaifyDatabase/IImageData.h
        DESTINATION MosaifyDatabase/include/MosaifyDatabase)

install(TARGETS MosaifyDB
        ARCHIVE DESTINATION MosaifyDatabase/lib
        LIBRARY DESTINATION MosaifyDatabase/lib
        RUNTIME DESTINATION MosaifyDatabase/bin
)
