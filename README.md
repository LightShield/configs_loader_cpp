# ConfigsLoader C++

A modern, compile-time optimized configuration management library for C++20.

## TL;DR

ConfigsLoader delivers **high-performance configuration management** through compile-time optimization, while providing **exceptional developer experience** with type-safe hierarchical configs and automatic validation, and **superior end-user experience** with auto-generated interactive help and flexible input methods.

**Performance**: Direct memory access (no hash maps, no lookups, no virtual dispatch)  
**Developer**: Type-safe structs, compile-time validation, hierarchical configs with partial initialization  
**End-User**: Interactive help with current values, preset files, comprehensive error messages

## Design Philosophy

### Make the Common Case Fast

**The common case**: Reading configuration values during program execution.

Most config libraries optimize for parsing (happens once at startup) at the expense of reading (happens thousands of times during execution). This inverts this priority:

```cpp
// Hot path - zero overhead, direct memory access
int timeout = loader.configs.server.timeout.value;
```

**No hash maps. No string lookups. No pointer indirection.** Just direct struct member access that compiles to a single `ldr` instruction.

### Utilize Compile-Time Knowledge

Your configuration structure is known at compile time. This leverages this for:

- **Type safety**: Compiler catches typos and type mismatches
- **Zero-cost abstractions**: All indirection optimized away
- **Automatic help generation**: Structure known, help auto-generated
- **Validation**: Default values and constraints checked at compile time where possible

```cpp
struct MyConfig {
    Config<int> port{
        .default_value = 8080,           // Compile-time constant
        .flags = {"--port", "-p"},       // Compile-time array
        .required = true,                // Compile-time flag
        .verifier = [](int p) { return p > 0 && p < 65536; }  // Inline lambda
    };
    REGISTER_CONFIG_FIELDS(port)  // Compile-time registration
};
```

### Runtime Flexibility When Needed

While optimizing for compile-time knowledge, the library supports runtime scenarios:

- **Human interaction**: `--help` shows current state after loading configs
- **Preset files**: Load configurations from external files
- **CLI overrides**: Command-line arguments override presets
- **Runtime updates**: Configs can be modified during execution (rare)
- **Serialization**: Dump current state for debugging or persistence

The key: **optimize for the common case (reading), support the rare case (updating and human interaction)**.

## Features by User

### For Performance-Critical Applications

**Direct Memory Access - No Indirection**

```cpp
// Compiles to: ldr w0, [x0, #96]
int value = loader.configs.server.port.value;
```

No hash maps, no string comparisons, no virtual dispatch, no pointer chasing. Just direct struct member access.

**Assembly-Verified Zero Cost**

All convenience features (implicit conversion, helper methods) compile to identical assembly as direct access:

```cpp
// All three compile to identical assembly:
int v1 = loader.configs.group.config.value.value;
const NestedConfig& nested = loader.configs.group;  // Implicit conversion
int v2 = nested.value.value;
int v3 = loader.configs.group.get_name().empty() ? 0 : 1;  // get_name() inlined
```

**Compile-Time Optimization**

- Constexpr default values
- Inline validation lambdas
- Template-based dispatch (no virtual calls)
- Zero-size abstractions

### For Developers

**Type-Safe Hierarchical Configurations**

```cpp
struct DatabaseConfig {
    Config<std::string> host{.default_value = "localhost"};
    Config<int> port{.default_value = 5432};
    REGISTER_CONFIG_FIELDS(host, port)
};

struct ServerConfig {
    ConfigGroup<DatabaseConfig> primary_db{.name_ = "primary_db"};
    ConfigGroup<DatabaseConfig> replica_db{.name_ = "replica_db"};
    REGISTER_CONFIG_FIELDS(primary_db, replica_db)
};
```

**Compile-Time Name Validation**

```cpp
// Typo caught at compile time:
int port = loader.configs.server.prot.value;  // ❌ Compile error
int port = loader.configs.server.port.value;  // ✅ Compiles
```

IDE autocomplete works perfectly - your editor knows the structure.

**Partial Initialization**

Override only what differs from defaults:

```cpp
struct AppConfig {
    ConfigGroup<ServerConfig> api_server{
        .config = {
            .primary_db = {
                .config = {
                    .port = {.default_value = 5433}  // Override just this
                }
            }
        },
        .name_ = "api_server"
    };
    REGISTER_CONFIG_FIELDS(api_server)
};
```

**Built-in Input Validation**

```cpp
Config<int> port{
    .default_value = 8080,
    .required = true,
    .verifier = [](int p) { return p > 0 && p < 65536; }
};

Config<std::string> email{
    .default_value = "",
    .verifier = [](const std::string& e) { return e.find('@') != std::string::npos; }
};
```

Validation runs automatically during init. All errors collected and reported together.

**Implicit Conversion for Cleaner Code**

```cpp
// Verbose:
configure_database(loader.configs.server.config.database.config);

// Clean:
const ServerConfig& server = loader.configs.server;
configure_database(server.database);  // Implicit conversion!
```

### For End Users

**Interactive Help System**

```bash
# Basic help
./myapp --help

# Filtered help
./myapp --help required      # Show only required fields
./myapp --help database      # Show only database group
./myapp --help groups        # Show group structure
./myapp --help all           # Show everything
```

**Current Values in Help**

```bash
# Load preset and see what's actually configured
./myapp --preset prod.toml --port 9090 --help all
```

Output shows:
```
--port  <int>  Server port (current: 9090, default: 8080)
--host  <string>  Server host (default: "localhost")
```

**Comprehensive Error Messages**

```
Configuration validation failed with 3 error(s):

  • Required field '--database.host' is not set (flag: --database.host)
  • Required field '--database.port' is not set (flag: --database.port)
  • Required field '--api-key' is not set (flag: --api-key)
```

All errors reported at once, with clear instructions.

**Flexible Input Methods**

```bash
# Preset file with CLI overrides
./myapp --preset production.toml --port 9090

# Pure CLI
./myapp --host localhost --port 8080 --verbose true

# Mixed formats
./myapp --host=localhost -p 8080 --verbose true
```

**Multiple Output Formats**

```cpp
// Dump current configuration
std::cout << loader.dump_configs(SerializationFormat::CLI);
std::cout << loader.dump_configs(SerializationFormat::TOML);

// Dump only changes from defaults
std::cout << loader.dump_configs(SerializationFormat::TOML, true);
```

## Quick Start

```cpp
#include "configs_loader.hpp"

struct MyConfig {
    Config<std::string> input{
        .default_value = "input.txt",
        .flags = {"--input", "-i"},
        .required = true
    };
    REGISTER_CONFIG_FIELDS(input)
};

int main(int argc, char* argv[]) {
    ConfigsLoader<MyConfig> loader;
    if (loader.init(argc, argv) != 0) {
        return 1;
    }
    
    std::string input = loader.configs.input.value;
    // ... use config
    return 0;
}
```

## Advanced Features

### Preset Files

```toml
# production.toml
host = "prod.example.com"
port = 5432
timeout = 60
```

```bash
./myapp --preset production.toml --port 5433  # Override port
```

### Custom Help Formatting

```cpp
HelpFormat custom{
    .program_name = "my-tool",
    .use_colors = false,
    .max_width = 100,
    .show_current_values = true
};

std::string help = loader.generate_help("", std::cref(custom));
```

### Validation with Custom Logic

```cpp
Config<std::string> url{
    .default_value = "",
    .required = true,
    .verifier = [](const std::string& u) {
        return u.starts_with("http://") || u.starts_with("https://");
    }
};
```

## Performance Characteristics

| Operation | Complexity | Notes |
|-----------|-----------|-------|
| Read config value | O(1) | Direct memory access |
| Init (parse + validate) | O(n + f) | n=argc, f=fields (once at startup) |
| Help generation | O(f) | f=fields (only when --help used) |
| Serialization | O(f) | f=fields (only when explicitly called) |

**Hot path (reading configs)**: Single instruction, zero overhead.

## Module Architecture

```
┌─────────────────────────────────────────────────────────┐
│                   ConfigsLoader                         │
│                   (Orchestrator)                        │
└────────────┬────────────────────────────────────────────┘
             │
    ┌────────┼────────┬──────────┬─────────────┐
    │        │        │          │             │
┌───▼───┐ ┌─▼──┐ ┌───▼────┐ ┌───▼──────┐ ┌───▼────────┐
│  CLI  │ │Help│ │Serial- │ │Validation│ │   Config   │
│Parser │ │Gen │ │ization │ │          │ │ (Core)     │
└───────┘ └────┘ └────────┘ └──────────┘ └────────────┘
```

Each module is independently testable and has a single responsibility.

## Design Decisions

### Why Not Singletons?

Singletons prevent multiple configurations, complicate testing, and create initialization order issues. The design prefers: initialize once in main, pass by reference.

### Why Not Runtime Reflection?

Runtime reflection requires hash maps, string lookups, and type erasure. The library uses compile-time structure for zero-cost abstractions.

### Why Struct-Based?

```cpp
// Compile-time checked, IDE autocomplete, type-safe
int port = loader.configs.server.port.value;

// vs runtime string-based (errors at runtime, no autocomplete)
int port = loader.get<int>("server.port");
```

### Why No Inheritance for Config/ConfigGroup?

C++ doesn't allow designated initializers on types with base classes. The design chooses clean initialization syntax over avoiding function overloads.

## Future (C++26 Reflection)

- Automatic field registration (no macros)
- Compile-time flag conflict detection
- Compile-time help generation

## Building

```bash
mkdir build && cd build
cmake .. -DENABLE_TOML_PRESETS=ON
cmake --build .
ctest  # Run 62 tests
```

## Requirements

- C++20 or later
- CMake 3.14+
- Optional: toml++ for TOML preset support

## License

See LICENSE file.
