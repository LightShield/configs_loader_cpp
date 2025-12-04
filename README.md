# ConfigsLoader C++

A modern, compile-time optimized configuration management library for C++20.

## TL;DR

ConfigsLoader delivers **high-performance configuration management** through direct memory access and minimal runtime footprint, **exceptional developer experience** with type-safe hierarchical configs and compile-time validation, and **superior end-user experience** with auto-generated interactive help, input validation with clear error messages, and flexible configuration sources.

**Performance**: Direct memory access + minimal memory footprint after init  
**Developer**: Type-safe structs, hierarchical configs, input validation per field  
**End-User**: Interactive help, preset files, validation errors with descriptions

## Design Philosophy

### Optimize for the Hot Path

In my experience with configuration libraries, I haven't found one that provides the combination of features needed: high-performance for reading configs (the hot path), ease of use for developers writing the code, and excellent auto-generated user experience.

**The hot path**: Reading configuration values during program execution.

```cpp
// Hot path - single instruction: ldr w0, [x0, #96]
int timeout = loader.configs.server.timeout.value;
```

Configuration structure is known at compile time, yet most implementations I've encountered use runtime lookups - searching for config names in maps or similar structures. This library utilizes compile-time knowledge for direct memory access.

### Focus on User Experience for Non-Critical Paths

While optimizing the hot path (reading configs, compile-time usage while writing code), the library focuses on ease of use for non-performance-critical scenarios like initialization and human interaction:

- **Init time**: Parse, validate, apply (happens once, ~1ms)
- **Human interaction**: `--help`, error messages, config exploration
- **Updates**: Runtime config modifications (rare, outside hot path)

These happen outside the critical path, so the focus is on usability, not performance.

### Ephemeral Initialization Objects

Initialization components are created on-demand within function scope and destroyed before the "real program" starts:

```cpp
// In init():
{
    ConfigValidator validator(configs);      // Created
    validator.validate_reserved_flags();     // Used
}   // Destroyed - memory freed before program runs

CliArgumentParser::parse(argc, argv);        // Static, no state
{
    ConfigApplier applier(configs);          // Created
    applier.apply_from_cli(flags);           // Used
}   // Destroyed - memory freed before program runs
```

**Why**: Minimize the real program's memory footprint. Initialization is ephemeral; execution is persistent. Optimize for the running state.

## Features by User

### For Performance-Critical Applications

**Direct Memory Access**

```cpp
// Compiles to: ldr w0, [x0, #96]
int value = loader.configs.server.port.value;
```

Direct struct member access. The compiler optimizes this to a single load instruction with a known offset.

**Minimal Runtime Footprint**

- **During init**: Parser, validator, applier (~1-2KB total)
- **After init**: Only config structs remain (~few KB)
- **During execution**: Zero allocation, zero lookup overhead

**Assembly-Verified Zero Cost**

All convenience features compile to identical assembly as direct access. Verified via assembly inspection.

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

IDE autocomplete works perfectly - the compiler knows the structure.

**Partial Initialization**

Override only what differs from defaults:

```cpp
ConfigGroup<ServerConfig> api_server{
    .config = {
        .port = {.default_value = 8080}  // Override just port
    },
    .name_ = "api_server"
};
```

**Input Validation Per Field**

Validation runs during value updates (primarily during init). Failed validations are reported to the user with config descriptions:

```cpp
Config<int> port{
    .default_value = 8080,
    .flags = {"--port"},
    .description = "Server listening port",
    .required = true,
    .verifier = [](int p) { return p > 0 && p < 65536; }
};
```

**All Errors Reported Together**

Solves the frustrating flow of "fix 1 issue → rerun → next error → rerun → next error...". All validation errors collected and shown at once:

```
Configuration validation failed with 2 error(s):

  • Required field '--host' is not set [Server hostname]
  • Required field '--port' is not set [Server listening port]

Configuration application failed with 1 error(s):

  • Validation failed for field '--timeout' [Request timeout]: value = -5
```

### For End Users

**Interactive Help System**

```bash
./myapp --help              # Navigation help
./myapp --help all          # All options (grep-friendly)
./myapp --help required     # Only required fields
./myapp --help database     # Only database group
./myapp --help groups       # Group structure
```

**Current Values in Help**

After loading preset and CLI args, `--help` shows current state:

```bash
./myapp --preset prod.toml --port 9090 --help all
```

Output:
```
--port  <int>  Server port (current: 9090, default: 8080)
--host  <string>  Server host (default: "localhost")
```

**Preset Files as Base Configuration**

Use preset files as base config, override with CLI (CLI has precedence):

```bash
# Load base config, override specific values
./myapp --preset production.toml --port 9090 --verbose true
```

**Print Current Configuration**

Print current config to stdout (useful for creating preset files):

```bash
# Print current config after loading and validation
./myapp --preset base.toml --port 9090 --print-config > current.toml

# Print only changes from defaults
./myapp --preset base.toml --port 9090 --verbose true --print-config > changes.toml
```

**Preset Building Loop**

Iteratively build and refine preset files:

```bash
# 1. Start with defaults
./myapp --print-config > base.toml

# 2. Load base, override, save new preset
./myapp --preset base.toml --port 9090 --timeout 60 --print-config > prod.toml

# 3. Use production preset
./myapp --preset prod.toml
```

**Comprehensive Error Messages**

All errors reported together with config descriptions (in brackets):

```
Configuration validation failed with 2 error(s):

  • Required field '--database.host' is not set [Database server hostname]
  • Required field '--api-key' is not set [API authentication key]

Configuration application failed with 2 error(s):

  • Validation failed for field '--port' [Server port]: value = 99999
  • Validation failed for field '--email' [Contact email]: value = invalid
```

Eliminates the frustrating "fix one, rerun, fix next, rerun..." cycle.

**Flexible Input Methods**

```bash
./myapp --preset production.toml --port 9090
./myapp --host localhost --port 8080 --verbose true
./myapp --host=localhost -p 8080 --verbose true
```

## Quick Start

```cpp
#include "configs_loader.hpp"

struct MyConfig {
    Config<std::string> input{
        .default_value = "input.txt",
        .flags = {"--input", "-i"},
        .required = true,
        .description = "Input file path"
    };
    REGISTER_CONFIG_FIELDS(input)
};

int main(int argc, char* argv[]) {
    ConfigsLoader<MyConfig> loader;
    if (loader.init(argc, argv) != 0) {
        return 1;  // Errors printed to stderr
    }
    
    std::string input = loader.configs.input.value;
    // ... use config
    return 0;
}
```

## Performance Characteristics

| Operation | Complexity | Notes |
|-----------|-----------|-------|
| Read config value | O(1) | Direct memory access, single instruction |
| Init (parse + validate) | O(n + f) | n=argc, f=fields (once at startup) |
| Help generation | O(f) | f=fields (only when --help used) |
| Serialization | O(f) | f=fields (only when explicitly called) |

**Hot path (reading configs)**: Single instruction, zero overhead.

## Module Architecture

```
┌─────────────────────────────────────────────────────────┐
│                   ConfigsLoader                         │
│              (Orchestrator - Stateless)                 │
│  Creates helpers on-demand, discards after init()       │
└────────────┬────────────────────────────────────────────┘
             │
    ┌────────┼────────┬──────────┬─────────────┐
    │        │        │          │             │
┌───▼───┐ ┌─▼──┐ ┌───▼────┐ ┌───▼──────┐ ┌───▼────────┐
│  CLI  │ │Help│ │Serial- │ │Validation│ │   Config   │
│Parser │ │Gen │ │ization │ │          │ │ (Core)     │
│       │ │    │ │        │ │          │ │            │
│Static │ │On- │ │Factory │ │On-demand │ │Persistent  │
│       │ │demand│ │+Strategy│ │         │ │(Runtime)   │
└───────┘ └────┘ └────────┘ └──────────┘ └────────────┘
```

**Initialization (Once)**: Parser, Validator, Applier created and destroyed  
**Runtime (Always)**: Only Config structs + `m_initialized` flag remain  
**On-Demand**: Help generator, serializers created when needed

Serialization uses Factory pattern for creation and Strategy pattern for format extensibility.

Each module is independently testable. Initialization helpers are ephemeral to minimize the real program's memory footprint.

## Design Decisions

### Why Not Singletons?

Singletons require checking `is_initialized()` before every API call - a cost paid on every access in the hot path:

```cpp
// Singleton pattern:
Config& Config::instance() {
    if (!initialized) { initialize(); }  // Branch on every call
    return config;
}
```

Even with branch prediction, this is wasted work. Instead, ConfigsLoader is statically initialized with the default constructor (compiler has an address), then user-initialized with `init(argc, argv)`. If not initialized, it runs with default values.

### Why Not Runtime Reflection?

Runtime reflection requires hash maps, string lookups, and type erasure. This library uses compile-time structure for zero-cost abstractions.

### Why Struct-Based?

Allows the compiler to optimize memory accesses:

```cpp
// Compile-time checked, IDE autocomplete, direct memory access
int port = loader.configs.server.port.value;

// vs runtime string-based (hash map lookup, no compile-time checks)
int port = loader.get<int>("server.port");
```

### Why Ephemeral Initialization Objects?

Optimized for the actual program's running state. Initialization happens once (~1ms), program runs for hours/days. Keep only what's needed during execution.

### Why No Inheritance for Config/ConfigGroup?

C++ doesn't allow designated initializers on types with base classes. The design chooses clean initialization syntax over avoiding function overloads. Additionally, vtable overhead is unnecessary for compile-time known structures - no need to pay for runtime polymorphism.

## Advanced Features

### Print Configuration

```cpp
// Print current config to stdout
loader.print_config();  // TOML format by default

// Print in CLI format
loader.print_config(SerializationFormat::CLI);

// Print only changes
loader.print_config(SerializationFormat::TOML, true);
```

Pipe to file for preset creation:
```bash
./myapp --port 9090 --print-config > my-preset.toml
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
    .description = "API endpoint URL",
    .required = true,
    .verifier = [](const std::string& u) {
        return u.starts_with("http://") || u.starts_with("https://");
    }
};
```

## Building

```bash
mkdir build && cd build
cmake .. -DENABLE_TOML_PRESETS=ON
cmake --build .
ctest  # Run 63 tests
```

## Requirements

- C++20 or later
- CMake 3.14+
- Optional: toml++ for TOML preset support

## Future (C++26 Reflection)

- Automatic field registration (no macros)
- Compile-time flag conflict detection
- Compile-time help generation

## License

See LICENSE file.
