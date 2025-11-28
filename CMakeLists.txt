cmake_minimum_required(VERSION 3.15)
project(configs_loader_cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Library
add_library(configs_loader_cpp INTERFACE)

target_include_directories(configs_loader_cpp
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Example
add_executable(basic_example
    examples/basic_example.cpp
)

target_link_libraries(basic_example
    PRIVATE
        configs_loader_cpp
)

# Compiler warnings
if(MSVC)
    target_compile_options(basic_example PRIVATE /W4)
else()
    target_compile_options(basic_example PRIVATE 
        -Wall -Wextra -Wpedantic -Wshadow
        -Wconversion -Wsign-conversion
        -Wfloat-equal -Wold-style-cast
        -Woverloaded-virtual -Wnon-virtual-dtor
        -Wzero-as-null-pointer-constant
        -Wmissing-declarations -Wformat=2
        -Wswitch-enum -Wundef
    )
endif()
