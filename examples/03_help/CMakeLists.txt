add_executable(help_example help_example.cpp)

target_link_libraries(help_example
    PRIVATE
        configs_loader_cpp
)

target_compile_options(help_example PRIVATE ${WARNING_FLAGS})
