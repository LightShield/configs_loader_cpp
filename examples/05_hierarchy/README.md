# Hierarchical Configs Example

Demonstrates how to use `ConfigGroup` to create nested configuration structures with automatic prefix handling.

## Features

- **Nested config structures**: Organize related configs into reusable groups
- **Automatic prefix generation**: Use `CONFIG_GROUP(Type, name)` macro to auto-generate prefixes from variable names
- **Multi-level hierarchy**: ConfigGroups can contain other ConfigGroups
- **Composition-based access**: Access nested configs via `.config` member
- **Prefixed CLI flags**: Flags automatically include full path (e.g., `--backend.primary_db.host`)

## Structure

```cpp
DatabaseConfig
  ├── host
  └── port

ServerConfig
  ├── primary_db (ConfigGroup<DatabaseConfig>)
  ├── replica_db (ConfigGroup<DatabaseConfig>)
  └── timeout

AppConfig
  ├── backend (ConfigGroup<ServerConfig>)
  └── app_name
```

## Usage

```bash
# Show help with prefixed flags
./hierarchy_example --help

# Set nested config values
./hierarchy_example \
  --name myapp \
  --backend.primary_db.host db1.example.com \
  --backend.primary_db.port 3306 \
  --backend.replica_db.host db2.example.com \
  --backend.timeout 60
```

## Access Patterns

```cpp
// Direct access via .config member
loader.configs.app_name.value
loader.configs.backend.config.timeout.value
loader.configs.backend.config.primary_db.config.host.value

// Using scoped alias for cleaner access
const auto& backend = loader.configs.backend.config;
backend.timeout.value
backend.primary_db.config.host.value
```

## Key Points

- Each config struct calls `REGISTER_CONFIG_FIELDS` for its own fields
- `CONFIG_GROUP(Type, name)` automatically uses variable name as group name
- ConfigGroup uses composition with `.config` member to enable designated initializers
- Flags in nested configs don't include prefix - it's applied automatically
- Minimal overhead - one pointer per ConfigGroup for name storage
- Group name stored as `name_` (trailing underscore) to avoid collisions
- Prefixes are accumulated for multi-level hierarchies (e.g., `backend.primary_db.host`)
- Use scoped aliases (`const auto&`) for cleaner access to deeply nested configs
