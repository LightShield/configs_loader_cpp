add_library(configs_loader_warnings INTERFACE)

if(MSVC)
    target_compile_options(configs_loader_warnings INTERFACE /W4 /WX /permissive-)
else()
    target_compile_options(configs_loader_warnings INTERFACE
        -Werror
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wsign-conversion
        -Woverloaded-virtual
        -Wnon-virtual-dtor
        -Wformat=2
        -Wunused
        # -Wpadded is disabled for this library as layout depends on template parameters
    )
endif()

# Alias for namespaced usage if needed by examples
if(NOT TARGET lightshield::configs_loader_warnings)
    add_library(lightshield::configs_loader_warnings ALIAS configs_loader_warnings)
endif()
