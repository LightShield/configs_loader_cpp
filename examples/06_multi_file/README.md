# Multi-File Hierarchical Configs Example

Demonstrates organizing hierarchical configs across multiple files, where each module defines its own config struct independently.

## File Structure

```
multi_file/
├── config/
│   └── app_config.hpp         # Main app config (aggregates all)
├── src/
│   ├── database/
│   │   ├── database_config.hpp # Database config definition
│   │   ├── database.hpp        # Database class
│   │   └── database.cpp        # Database implementation
│   ├── cache/
│   │   ├── cache_config.hpp    # Cache config definition
│   │   ├── cache.hpp           # Cache class
│   │   └── cache.cpp           # Cache implementation
│   ├── logging/
│   │   ├── logging_config.hpp  # Logging config definition
│   │   ├── logger.hpp          # Logger class
│   │   └── logger.cpp          # Logger implementation
│   └── server/
│       └── server_config.hpp   # Server config (aggregates db + cache)
└── main.cpp                    # Main application
```

**Key principle**: Each module defines its own config independently and is unaware of where it sits in the hierarchy.

## Config Hierarchy

```
AppConfig
├── server (ServerConfig)
│   ├── primary_db (DatabaseConfig)
│   ├── replica_db (DatabaseConfig)
│   ├── cache (CacheConfig)
│   ├── port
│   └── timeout
├── logging (LoggingConfig)
├── app_name
└── environment
```

## Module-Agnostic Pattern

Modules only know about their own config, not the full hierarchy:

```cpp
// database.hpp - Module only knows about its own config
class Database {
public:
    explicit Database(const DatabaseConfig& config);
    void connect();
private:
    const DatabaseConfig& config_;  // Unaware of hierarchy
};

// main.cpp - Passes nested config to module
Database primary_db(loader.configs.server.config.primary_db.config);
primary_db.connect();  // Module uses config internally
```

## Usage

```bash
./multi_file_example \
  --name myapp \
  --server.port 9000 \
  --server.primary_db.host db1.prod.com \
  --server.cache.ttl 7200 \
  --logging.level 3
```

## Key Benefits

1. **Separation of Concerns**: Each module defines its own config
2. **Reusability**: DatabaseConfig used for both primary_db and replica_db
3. **Module Agnostic**: Modules don't need to know about the full hierarchy
4. **Clean Organization**: Config structure mirrors application architecture

See `real_world/` example for more advanced patterns including sub-modules, multiple instances with different defaults, and various config passing strategies.
