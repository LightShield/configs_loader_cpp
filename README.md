# configs_loader_cpp

Configuration loader library for C++ projects.

## Features

- Clean designated initializer syntax (C++20)
- CLI argument parsing (multiple formats: `--key value`, `-k value`, `--key=value`)
- Required field validation
- Type conversion (string, int, bool, double)
- Custom verifier functions
- Reserved `--preset`/`-p` flags for future JSON preset support

## Basic Usage

```cpp
#include "configs_loader.hpp"

struct MyConfigs {
    Config<std::string> filename{
        .default_value = "input.txt",
        .flags = {"--file", "-f"},
        .required = true
    };
    
    Config<int> log_level{
        .default_value = 2,
        .flags = {"--log-level", "-l"}
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

## Future (C++26)

- `REGISTER_CONFIG_STRUCT(StructName)` - Auto-detect fields via reflection
- Compile-time preset flag validation
- Constexpr flag containers
