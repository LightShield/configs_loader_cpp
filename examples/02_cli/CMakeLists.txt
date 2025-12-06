add_executable(cli_example cli_example.cpp)

target_link_libraries(cli_example
    PRIVATE
        configs_loader_cpp
)

target_compile_options(cli_example PRIVATE ${WARNING_FLAGS})
