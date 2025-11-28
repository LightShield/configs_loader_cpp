# configs_loader_cpp

Header-only C++20 configuration loader library with clean API separation.

## Features

- ✅ Header-only library (no linking required)
- ✅ Clean public API (`configs_loader.hpp`) with implementation details hidden
- ✅ Automatic `--help`/`-h` handling (prints help and exits)
- ✅ Clean designated initializer syntax (C++20)
- ✅ CLI argument parsing (multiple formats: `--key value`, `-k value`, `--key=value`)
- ✅ Column-aligned help output with text wrapping
- ✅ Config dumping (all values or only changes from defaults)
- ✅ Required field validation
- ✅ Type conversion (string, int, bool, double)
- ✅ Custom verifier functions
- ✅ Two-phase initialization (construction never throws)
- ✅ Reserved `--preset`/`-p` flags for future JSON preset support
- ✅ Initialization safety checks

## Library Structure

The library is split into two headers for clean API separation:

- **`configs_loader.hpp`** - Public API (include this in your code)
  - Clean interface showing only public methods
  - Well-documented API surface
  - Hides implementation details

- **`configs_loader_impl.hpp`** - Implementation details (automatically included)
  - Template implementations
  - Helper functions
  - Internal utilities
  - Users should not include this directly

This separation keeps the public API clean and easy to understand while maintaining the header-only design.

## Installation

Header-only library - just copy `include/` directory to your project.

Or use CMake:

```bash
# As subdirectory
add_subdirectory(configs_loader_cpp)
target_link_libraries(your_target PRIVATE configs_loader_cpp)

# Or install headers
cmake -B build
cmake --install build --prefix /usr/local
```

## Quick Start

```cpp
#include "configs_loader.hpp"

struct MyConfigs {
    Config<std::string> filename{
        .default_value = "input.txt",
        .flags = {"--file", "-f"},
        .required = true,
        .description = "Input file to process"
    };
    
    Config<int> log_level{
        .default_value = 2,
        .flags = {"--log-level", "-l"},
        .description = "Logging verbosity level (0-5)"
    };

    REGISTER_CONFIG_FIELDS(filename, log_level)
};

int main(int argc, char* argv[]) {
    ConfigsLoader<MyConfigs> loader;
    
    try {
        loader.Init(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Configuration error: " << e.what() << "\n";
        return 1;
    }
    
    // Usage never throws
    std::cout << loader.configs.filename.value << "\n";
    std::cout << loader.configs.log_level.value << "\n";
    
    return 0;
}
```

## Global Config Pattern

For accessing configs across multiple files without passing them around:

**global_configs.hpp:**
```cpp
#pragma once
#include "configs_loader.hpp"

struct AppConfigs {
    Config<std::string> filename{...};
    Config<int> log_level{...};
    REGISTER_CONFIG_FIELDS(filename, log_level)
};

extern ConfigsLoader<AppConfigs> g_config_loader;

inline AppConfigs& GetConfigs() {
    return g_config_loader.configs;
}
```

**main.cpp:**
```cpp
#include "global_configs.hpp"

ConfigsLoader<AppConfigs> g_config_loader;  // Define once

int main(int argc, char* argv[]) {
    g_config_loader.Init(argc, argv);
    // Use GetConfigs() anywhere
}
```

**other_file.cpp:**
```cpp
#include "global_configs.hpp"

void some_function() {
    auto filename = GetConfigs().filename.value;  // Access from anywhere
}
```

## API Reference

### Config<T>

Template struct for defining configuration fields.

**Members:**
- `T default_value` - Default value for the config
- `std::vector<std::string> flags` - CLI flags (e.g., `{"--file", "-f"}`)
- `bool required` - Whether field must be explicitly set
- `std::function<bool(const T&)> verifier` - Custom validation function
- `T value` - Current value (direct access, never throws)
- `bool is_set()` - Whether value was explicitly set via CLI

### ConfigsLoader<ConfigsType>

Template class for loading and managing configurations.

**Methods:**
- `ConfigsLoader()` - Default constructor (never throws)
- `ConfigsLoader(int argc, char* argv[])` - Convenience constructor (calls Init)
- `void Init(int argc, char* argv[])` - Initialize from CLI args (can throw)

**Exceptions:**
- `std::runtime_error` - Required field not set, invalid preset flags, or parse errors

## Building Tests and Examples

By default, only the library headers are available. Tests and examples are disabled for library usage.

**Build with tests:**
```bash
cmake -B build -DBUILD_TESTS=ON
cmake --build build
ctest --test-dir build
```

**Build with examples:**
```bash
cmake -B build -DBUILD_EXAMPLES=ON
cmake --build build
./build/examples/basic_example --help
```

**Build both:**
```bash
cmake -B build -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
cmake --build build
```

See [examples/README.md](examples/README.md) and [tests/README.md](tests/README.md) for details.

## Future (C++26)

- `REGISTER_CONFIG_STRUCT(StructName)` - Auto-detect fields via reflection
- Compile-time preset flag validation
- Constexpr flag containers
