# Multi-File Hierarchical Configs Example

This example demonstrates how to organize hierarchical configs across multiple files, following a pattern where each module defines its own config struct, and these are aggregated hierarchically in the main application config.

## File Structure

```
multi_file/
├── database_config.hpp    # Database module config
├── cache_config.hpp       # Cache module config
├── logging_config.hpp     # Logging module config
├── server_config.hpp      # Server config (aggregates database + cache)
├── app_config.hpp         # Main app config (aggregates server + logging)
└── multi_file_example.cpp # Main application
```

## Config Hierarchy

```
AppConfig
├── server (ServerConfig)
│   ├── primary_db (DatabaseConfig)
│   │   ├── host
│   │   ├── port
│   │   └── username
│   ├── replica_db (DatabaseConfig)
│   │   ├── host
│   │   ├── port
│   │   └── username
│   ├── cache (CacheConfig)
│   │   ├── host
│   │   ├── port
│   │   └── ttl
│   ├── port
│   └── timeout
├── logging (LoggingConfig)
│   ├── level
│   ├── output
│   └── timestamps
├── app_name
└── environment
```

## Usage

```bash
# Show help with all prefixed flags
./multi_file_example --help

# Configure with nested flags
./multi_file_example \
  --name myapp \
  --env production \
  --server.port 9000 \
  --server.primary_db.host db1.prod.com \
  --server.primary_db.port 5432 \
  --server.replica_db.host db2.prod.com \
  --server.cache.host cache.prod.com \
  --server.cache.ttl 7200 \
  --logging.level 3 \
  --logging.output /var/log/app.log
```

## Access Patterns

### Direct Access
```cpp
loader.configs.server.primary_db.host.value
loader.configs.server.cache.ttl.value
loader.configs.logging.level.value
```

### Using References
```cpp
const auto& server = loader.configs.server;
std::cout << server.primary_db.host.value;
```

### Module-Agnostic Pattern
```cpp
// Module only knows about its own config type
void connect_database(const DatabaseConfig& db) {
    // Use db.host.value, db.port.value, etc.
}

// Pass nested config to module
connect_database(loader.configs.server.primary_db);
```

## Key Benefits

1. **Separation of Concerns**: Each module defines its own config independently
2. **Reusability**: DatabaseConfig used for both primary_db and replica_db
3. **Type Safety**: Compile-time checking of all config access
4. **Zero Overhead**: Direct member access via inheritance
5. **Module Agnostic**: Modules don't need to know about the full hierarchy
6. **Clean Organization**: Config structure mirrors application architecture
