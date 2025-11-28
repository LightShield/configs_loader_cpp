cmake_minimum_required(VERSION 3.15)
project(configs_loader_cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Fetch GTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Library
add_library(configs_loader_cpp INTERFACE)

target_include_directories(configs_loader_cpp
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Compiler warnings
set(WARNING_FLAGS
    -Werror
    -Wall
    -Wextra
    -Wshadow
    -Wconversion
    -Wsign-conversion
    -Wfloat-equal
    -Wold-style-cast
    -Woverloaded-virtual
    -Wnon-virtual-dtor
    -Wzero-as-null-pointer-constant
    -Wmissing-declarations
    -Wformat=2
    -Wswitch-enum
    -Wundef
)

# Tests
add_executable(config_tests
    tests/config_tests.cpp
)

target_link_libraries(config_tests
    PRIVATE
        configs_loader_cpp
        GTest::gtest_main
)

target_compile_options(config_tests PRIVATE ${WARNING_FLAGS})

add_executable(configs_loader_tests
    tests/configs_loader_tests.cpp
)

target_link_libraries(configs_loader_tests
    PRIVATE
        configs_loader_cpp
        GTest::gtest_main
)

target_compile_options(configs_loader_tests PRIVATE ${WARNING_FLAGS})

include(GoogleTest)
gtest_discover_tests(config_tests)
gtest_discover_tests(configs_loader_tests)

# Examples
add_subdirectory(examples)
