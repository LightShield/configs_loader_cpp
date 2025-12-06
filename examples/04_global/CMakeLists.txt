add_executable(global_example
    global_main.cpp
    other_file.cpp
)

target_link_libraries(global_example
    PRIVATE
        configs_loader_cpp
)

target_compile_options(global_example PRIVATE ${WARNING_FLAGS})
