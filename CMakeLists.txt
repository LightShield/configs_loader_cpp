cmake_minimum_required(VERSION 3.15)
project(ConfigsLoader VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Options ---
option(BUILD_TESTS "Build tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)
option(ENABLE_TOML_PRESETS "Enable TOML preset file support" ON)

# --- Helpers ---
include(cmake/compiler_warnings.cmake)

# --- Components ---

# Main Library (Aggregator/Interface)
add_library(configs_loader_cpp INTERFACE)

target_include_directories(configs_loader_cpp
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(configs_loader_cpp INTERFACE
    configs_loader_warnings
)

# Add implementation components
add_subdirectory(src)

# Main library depends on implementation
target_link_libraries(configs_loader_cpp INTERFACE
    configs_loader_preset_parser
)

# Alias for namespaced usage
if(NOT TARGET lightshield::configs_loader)
    add_library(lightshield::configs_loader ALIAS configs_loader_cpp)
endif()

# --- Optional Subdirectories ---

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# --- Installation ---
include(GNUInstallDirs)

# Install targets
install(TARGETS configs_loader_cpp configs_loader_preset_parser configs_loader_warnings
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
