cmake_minimum_required(VERSION 3.15)
project(configs_loader_cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Feature flags (default OFF for library usage)
option(BUILD_TESTS "Build tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)
option(ENABLE_TOML_PRESETS "Enable TOML preset file support" ON)

# Library
add_library(configs_loader_cpp INTERFACE)

target_include_directories(configs_loader_cpp
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Preset parser implementation (not header-only)
add_library(configs_loader_preset_parser
    src/serialization/preset_parser.cpp
)

target_include_directories(configs_loader_preset_parser
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(configs_loader_preset_parser PUBLIC cxx_std_20)

# Optional TOML support
if(ENABLE_TOML_PRESETS)
    message(STATUS "TOML preset support: ENABLED")
    
    # Add toml++ as submodule or fetch
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/tomlplusplus/CMakeLists.txt")
        include(FetchContent)
        FetchContent_Declare(
            tomlplusplus
            GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
            GIT_TAG v3.4.0
        )
        FetchContent_MakeAvailable(tomlplusplus)
    else()
        add_subdirectory(third_party/tomlplusplus)
    endif()
    
    target_sources(configs_loader_preset_parser PRIVATE
        src/serialization/toml_parser.cpp
    )
    
    target_link_libraries(configs_loader_preset_parser PUBLIC
        tomlplusplus::tomlplusplus
    )
    
    target_compile_definitions(configs_loader_preset_parser PUBLIC
        CONFIGS_LOADER_ENABLE_TOML
    )
else()
    message(STATUS "TOML preset support: DISABLED (use -DENABLE_TOML_PRESETS=ON to enable)")
endif()

target_link_libraries(configs_loader_cpp INTERFACE
    configs_loader_preset_parser
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

# Tests (optional)
if(BUILD_TESTS)
    # Fetch GTest
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)

    enable_testing()
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

add_executable(help_generator_tests
    tests/help/help_generator_tests.cpp
)

target_link_libraries(help_generator_tests
    PRIVATE
        configs_loader_cpp
        GTest::gtest_main
)

target_compile_options(help_generator_tests PRIVATE ${WARNING_FLAGS})

add_executable(cli_argument_parser_tests
    tests/cli/cli_argument_parser_tests.cpp
)

target_link_libraries(cli_argument_parser_tests
    PRIVATE
        configs_loader_cpp
        GTest::gtest_main
)

target_compile_options(cli_argument_parser_tests PRIVATE ${WARNING_FLAGS})

add_executable(toml_deserializer_tests
    tests/serialization/toml_deserializer_tests.cpp
)

target_link_libraries(toml_deserializer_tests
    PRIVATE
        configs_loader_cpp
        GTest::gtest_main
)

target_compile_options(toml_deserializer_tests PRIVATE ${WARNING_FLAGS})

include(GoogleTest)
gtest_discover_tests(config_tests)
gtest_discover_tests(configs_loader_tests)
gtest_discover_tests(cli_argument_parser_tests)
gtest_discover_tests(help_generator_tests)
gtest_discover_tests(toml_deserializer_tests)
endif()

# Examples (optional)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
