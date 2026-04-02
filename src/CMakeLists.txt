# Preset parser implementation
add_library(configs_loader_preset_parser
    serialization/preset_parser.cpp
)

target_include_directories(configs_loader_preset_parser
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(configs_loader_preset_parser PUBLIC cxx_std_20)
target_link_libraries(configs_loader_preset_parser PRIVATE configs_loader_warnings)

# Optional TOML support
if(ENABLE_TOML_PRESETS)
    # Add toml++ as submodule or fetch
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/tomlplusplus/CMakeLists.txt")
        include(FetchContent)
        FetchContent_Declare(
            tomlplusplus
            GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
            GIT_TAG v3.4.0
        )
        FetchContent_MakeAvailable(tomlplusplus)
    else()
        add_subdirectory(../third_party/tomlplusplus ${CMAKE_CURRENT_BINARY_DIR}/third_party/tomlplusplus)
    endif()
    
    target_sources(configs_loader_preset_parser PRIVATE
        serialization/toml_parser.cpp
    )
    
    target_link_libraries(configs_loader_preset_parser PUBLIC
        tomlplusplus::tomlplusplus
    )
    
    target_compile_definitions(configs_loader_preset_parser PUBLIC
        CONFIGS_LOADER_ENABLE_TOML
    )
endif()
