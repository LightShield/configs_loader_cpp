# ConfigsLoader C++

A modern, compile-time optimized configuration management library for C++20.

## Design Philosophy

### Make the Common Case Fast

**The common case**: Reading configuration values during runtime.

Most config libraries optimize for parsing (happens once) at the expense of reading (happens thousands of times). We flip this:

```cpp
// Common case - zero overhead
int timeout = loader.configs.server.timeout.value;  // Direct member access, no lookups
```

**No hash maps. No string lookups. No virtual dispatch.** Just direct struct member access.

### Utilize Domain Knowledge at Compile Time

We know your configuration structure at compile time. Why wait until runtime?

```cpp
struct MyConfig {
    Config<int> port{
        .default_value = 8080,
        .flags = {"--port", "-p"},
        .required = true,
        .description = "Server port",
        .verifier = [](int p) { return p > 0 && p < 65536; }
    };
    REGISTER_CONFIG_FIELDS(port)
};
```

**At compile time, we know:**
- Field names and types
- Default values
- Validation rules
- Help text
- Flag mappings

**At runtime, we just:**
- Parse arguments (once)
- Validate (once)
- Read values (fast, direct access)

### Zero Abstraction Cost

```cpp
// These compile to identical assembly:
int direct = my_struct.port;
int via_config = loader.configs.port.value;
const ServerConfig& server = loader.configs.server;  // Implicit conversion
```

Verified via assembly inspection - all abstractions optimize away completely.

## Key Features

### 1. Hierarchical Configurations

```cpp
struct DatabaseConfig {
    Config<std::string> host{.default_value = "localhost", .flags = {"--host"}};
    Config<int> port{.default_value = 5432, .flags = {"--port"}};
    REGISTER_CONFIG_FIELDS(host, port)
};

struct ServerConfig {
    ConfigGroup<DatabaseConfig> primary_db{.name_ = "primary_db"};
    ConfigGroup<DatabaseConfig> replica_db{.name_ = "replica_db"};
    REGISTER_CONFIG_FIELDS(primary_db, replica_db)
};

// Access: loader.configs.primary_db.host.value
// CLI: --primary_db.host localhost
```

### 2. Partial Initialization

Override only what you need:

```cpp
struct AppConfig {
    ConfigGroup<ServerConfig> api_server{
        .config = {
            .port = {.default_value = 8080}  // Override just port
        },
        .name_ = "api_server"
    };
    ConfigGroup<ServerConfig> admin_server{
        .config = {
            .port = {.default_value = 9090}  // Different port
        },
        .name_ = "admin_server"
    };
    REGISTER_CONFIG_FIELDS(api_server, admin_server)
};
```

### 3. Built-in Validation

```cpp
Config<int> port{
    .default_value = 8080,
    .flags = {"--port"},
    .required = true,
    .verifier = [](int p) { return p > 0 && p < 65536; }
};
```

Validation happens automatically. All errors collected and reported together:

```
Configuration validation failed with 3 error(s):

  • Required field '--host' is not set (flag: --host)
  • Required field '--port' is not set (flag: --port)
  • Required field '--database' is not set (flag: --database)
```

### 4. Automatic Help Generation

```cpp
ConfigsLoader<MyConfig> loader(argc, argv);
// --help automatically handled, shows:
// - All fields with descriptions
// - Default values
// - Current values (if different from default)
// - Required fields marked
// - Interactive filtering (--help required, --help <group>)
```

### 5. Multiple Serialization Formats

```cpp
// Dump current config
std::string cli_format = loader.dump_configs(SerializationFormat::CLI);
std::string toml_format = loader.dump_configs(SerializationFormat::TOML);

// Dump only changes
std::string changes = loader.dump_configs(SerializationFormat::TOML, true);
```

### 6. Preset Files

```cpp
// Load from TOML preset, override with CLI
./myapp --preset production.toml --port 9090
```

CLI arguments override preset values.

## Performance Characteristics

### Compile Time
- Type checking
- Default value validation
- Flag conflict detection (C++26)

### Init Time (Once)
- Parse CLI arguments: O(n) where n = argc
- Validate: O(fields)
- Apply values: O(fields)

### Runtime (Hot Path)
- Read value: **O(1)** - direct member access
- No allocations
- No lookups
- No virtual dispatch

## Usage

### Basic Example

```cpp
#include "configs_loader.hpp"

struct AppConfig {
    Config<std::string> input{
        .default_value = "input.txt",
        .flags = {"--input", "-i"},
        .required = true,
        .description = "Input file"
    };
    Config<bool> verbose{
        .default_value = false,
        .flags = {"--verbose", "-v"},
        .description = "Verbose output"
    };
    REGISTER_CONFIG_FIELDS(input, verbose)
};

int main(int argc, char* argv[]) {
    ConfigsLoader<AppConfig> loader;
    if (loader.init(argc, argv) != 0) {
        return 1;  // Errors printed to stderr
    }
    
    // Fast access - direct member access
    std::string input = loader.configs.input.value;
    bool verbose = loader.configs.verbose.value;
    
    // ... use configs
    return 0;
}
```

### Hierarchical Example

```cpp
struct PoolConfig {
    Config<int> min_connections{.default_value = 5, .flags = {"--min"}};
    Config<int> max_connections{.default_value = 20, .flags = {"--max"}};
    REGISTER_CONFIG_FIELDS(min_connections, max_connections)
};

struct DatabaseConfig {
    ConfigGroup<PoolConfig> pool{.name_ = "pool"};
    Config<std::string> host{.default_value = "localhost", .flags = {"--host"}};
    REGISTER_CONFIG_FIELDS(pool, host)
};

struct AppConfig {
    ConfigGroup<DatabaseConfig> database{.name_ = "database"};
    REGISTER_CONFIG_FIELDS(database)
};

// CLI: --database.pool.min 10 --database.pool.max 50
// Access: loader.configs.database.pool.min_connections.value

// Or use implicit conversion for cleaner code:
const DatabaseConfig& db = loader.configs.database;
const PoolConfig& pool = db.pool;
int min = pool.min_connections.value;
```

## Design Decisions

### Why Not Singletons?

Singletons prevent:
- Multiple configurations in one program
- Testing with different configs
- Thread-safe initialization

Our approach: Initialize once in main, pass by reference.

### Why Not Runtime Reflection?

Runtime reflection (like JSON libraries) requires:
- Hash map lookups for every access
- String comparisons
- Type erasure and casting
- Runtime overhead

We use compile-time structure for zero-cost abstractions.

### Why Struct-Based?

```cpp
// Type-safe, IDE-friendly, compile-time checked
int port = loader.configs.server.port.value;

// vs string-based (runtime errors, no autocomplete)
int port = loader.get<int>("server.port");
```

### Why Not Inheritance for Config/ConfigGroup?

C++ doesn't allow designated initializers on types with base classes. We choose clean initialization syntax over avoiding function overloads:

```cpp
// This beautiful syntax requires aggregates (no inheritance)
ConfigGroup<ServerConfig> api{
    .config = {
        .port = {.default_value = 8080}
    },
    .name_ = "api"
};
```

## Module Organization

```
include/
├── cli/                    # CLI argument parsing
│   ├── cli_argument_parser.hpp
│   └── config_applier.hpp
├── help/                   # Help generation
│   ├── help_generator.hpp
│   └── help_colors.hpp
├── serialization/          # Serialize/deserialize
│   ├── cli_serializer.hpp
│   ├── toml_serializer.hpp
│   └── preset_deserializer.hpp
├── validation/             # Configuration validation
│   └── config_validator.hpp
├── config.hpp              # Core Config/ConfigGroup types
└── configs_loader.hpp      # Main orchestrator
```

Each module has a single, clear responsibility.

## Future (C++26)

With reflection, we can move to compile time:
- Flag conflict detection (currently runtime)
- Automatic field registration (no REGISTER_CONFIG_FIELDS macro)
- Compile-time help generation

```cpp
// Future C++26:
struct MyConfig {
    Config<int> port{.default_value = 8080};
    Config<std::string> host{.default_value = "localhost"};
    // No REGISTER_CONFIG_FIELDS needed!
};
```

## Building

```bash
mkdir build && cd build
cmake .. -DENABLE_TOML_PRESETS=ON
cmake --build .
ctest
```

## Requirements

- C++20 or later
- CMake 3.14+
- Optional: toml++ for TOML preset support

## License

See LICENSE file.
