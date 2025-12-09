# Example 08: Enum Support

Demonstrates how to use enum types with configs_loader.

## Key Concepts

### Enum Configuration

Enums require two helper functions provided via `enum_traits`:
1. **Parser**: Converts string to enum (for CLI and TOML input)
2. **to_string**: Converts enum to string (for serialization and help text)

```cpp
Config<LogLevel> log_level{
    .default_value = LogLevel::Info,
    .enum_traits = {
        .parser = log_level_from_string,
        .to_string = [](const LogLevel& level) { return std::string(log_level_to_string(level)); }
    },
    .flags = {"--log-level", "-l"},
    .description = "Logging verbosity (error, warn, info, debug)"
};
```

### String-Based Input

Enums are **always treated as strings** in CLI and TOML:
- **CLI**: `--log-level debug` or `--log-level=debug`
- **TOML**: `log-level = "debug"`

Even if your enum has numeric values, you must provide string names for user input.

### Numeric Fallback

If you don't provide `to_string` in `enum_traits`, the enum will serialize as its numeric value:
- **With to_string**: `log-level = "debug"`
- **Without to_string**: `log-level = 3`

Always provide `to_string` for better usability.

## Running

```bash
# Default values
./enum_example

# Set log level
./enum_example --log-level debug

# Show help
./enum_example --help all

# Dump config
./enum_example --log-level error --print-config
```

## Output

```
Log Level: info
Output: output.txt

Dumped config (TOML):
log-level = "info"
output = "output.txt"
```
