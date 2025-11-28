add_executable(cli_example cli_example.cpp)
target_include_directories(cli_example PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(cli_example PRIVATE cxx_std_20)
