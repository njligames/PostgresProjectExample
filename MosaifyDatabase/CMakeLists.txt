option(BUILD_TESTS "Build the tests" ON)

find_package(PostgreSQL REQUIRED)

add_library(MosaifyDB STATIC
    MosaifyDatabase.cpp
    MosaifyDatabase.h)

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
