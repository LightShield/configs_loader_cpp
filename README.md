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
- ✅ **Hierarchical configs** with `ConfigGroup` and automatic prefix handling
- ✅ **TOML preset support** (optional, enabled via CMake flag)

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

## Hierarchical Configs

Use `ConfigGroup<T>` to create nested configuration structures with automatic prefix handling:

```cpp
struct DatabaseConfig {
    Config<std::string> host{.default_value = "localhost", .flags = {"--host"}};
    Config<int> port{.default_value = 5432, .flags = {"--port"}};
    REGISTER_CONFIG_FIELDS(host, port)
};

struct ServerConfig {
    CONFIG_GROUP(DatabaseConfig, primary_db);
    CONFIG_GROUP(DatabaseConfig, replica_db);
    Config<int> timeout{.default_value = 30, .flags = {"--timeout"}};
    REGISTER_CONFIG_FIELDS(primary_db, replica_db, timeout)
};

struct AppConfig {
    CONFIG_GROUP(ServerConfig, backend);
    Config<std::string> app_name{.default_value = "myapp", .flags = {"--name"}};
    REGISTER_CONFIG_FIELDS(backend, app_name)
};

// Usage
ConfigsLoader<AppConfig> loader;
loader.init(argc, argv);

// CLI: --backend.primary_db.host db1.example.com --backend.primary_db.port 3306

// Access via .config member
std::cout << loader.configs.backend.config.primary_db.config.host.value;

// Or use scoped alias for cleaner access
const auto& backend = loader.configs.backend.config;
std::cout << backend.primary_db.config.host.value;
```

**Key points:**
- `CONFIG_GROUP(Type, name)` macro automatically uses variable name as group name
- ConfigGroup uses composition with a `.config` member to enable designated initializers
- Access nested configs via `.config` or use scoped aliases for cleaner syntax
- Flags in nested configs don't include prefix - it's applied automatically during CLI parsing
- Minimal overhead - one pointer per ConfigGroup for name storage
- Group name stored as `name_` (trailing underscore) to avoid collisions with user fields
- Prefixes accumulate for multi-level hierarchies (e.g., `backend.primary_db.host`)
- Each config struct calls `REGISTER_CONFIG_FIELDS` for its own fields

See `examples/hierarchy/`, `examples/multi_file/`, and `examples/real_world/` for complete examples.

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

### ConfigGroup<T>

Template struct for creating hierarchical config structures using composition.

**Members:**
- `T config` - The nested config instance (access via `.config`)
- `std::string name_` - Group name used as prefix for nested flags (trailing underscore avoids collisions)

**Macro:**
- `CONFIG_GROUP(Type, name)` - Creates a ConfigGroup with automatic name from variable name

**Access Pattern:**
- Direct: `loader.configs.group_name.config.field.value`
- Scoped alias: `const auto& cfg = loader.configs.group_name.config;` then `cfg.field.value`

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
