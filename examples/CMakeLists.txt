cmake_minimum_required(VERSION 3.15)

# Basic example
add_executable(basic_example
    basic/basic_example.cpp
)

target_link_libraries(basic_example
    PRIVATE
        configs_loader_cpp
)

target_compile_options(basic_example PRIVATE ${WARNING_FLAGS})

# Global config example
add_executable(global_example
    global/global_main.cpp
    global/other_file.cpp
)

target_link_libraries(global_example
    PRIVATE
        configs_loader_cpp
)

target_compile_options(global_example PRIVATE ${WARNING_FLAGS})
