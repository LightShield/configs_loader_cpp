add_executable(real_world_example
    main.cpp
    src/database/database.cpp
    src/cache/cache.cpp
    src/server/server.cpp
)

target_include_directories(real_world_example PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(real_world_example PRIVATE configs_loader_cpp)
