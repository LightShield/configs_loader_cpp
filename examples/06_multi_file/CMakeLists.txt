add_executable(multi_file_example 
    multi_file_example.cpp
    src/database/database.cpp
    src/cache/cache.cpp
    src/logging/logger.cpp
)
target_link_libraries(multi_file_example PRIVATE configs_loader_cpp)
