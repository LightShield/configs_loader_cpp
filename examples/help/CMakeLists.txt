add_executable(help_example help_example.cpp)
target_include_directories(help_example PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(help_example PRIVATE cxx_std_20)
