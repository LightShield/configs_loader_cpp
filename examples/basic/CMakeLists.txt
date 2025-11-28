add_executable(basic_example
    basic_example.cpp
)

target_link_libraries(basic_example
    PRIVATE
        configs_loader_cpp
)

target_compile_options(basic_example PRIVATE ${WARNING_FLAGS})
