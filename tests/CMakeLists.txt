include(FetchContent)

# Fetch GoogleTest
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)

# Prevent GoogleTest from overriding our compiler/linker options
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Fetch and build GoogleTest
FetchContent_MakeAvailable(googletest)

# Add the test executable
add_executable(test_database_functions test_database_functions.cpp)

# Link GoogleTest and your library
target_link_libraries(test_database_functions gtest_main MosaifyDB)
target_include_directories(test_database_functions
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
        $<INSTALL_INTERFACE:include>
)

# Add a test
add_test(NAME test_database_functions COMMAND test_database_functions)