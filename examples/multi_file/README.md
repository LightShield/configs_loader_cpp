# Multi-File Hierarchical Configs Example

This example demonstrates how to organize hierarchical configs across multiple files, following a pattern where each module defines its own config struct, and these are aggregated hierarchically in the main application config.

## File Structure

```
multi_file/
├── config/
│   └── app_config.hpp         # Main app config (aggregates all)
├── src/
│   ├── database/
│   │   ├── database_config.hpp # Database config definition
│   │   ├── database.hpp        # Database class (uses DatabaseConfig)
│   │   └── database.cpp        # Database implementation
│   ├── cache/
│   │   ├── cache_config.hpp    # Cache config definition
│   │   ├── cache.hpp           # Cache class (uses CacheConfig)
│   │   └── cache.cpp           # Cache implementation
│   ├── logging/
│   │   ├── logging_config.hpp  # Logging config definition
│   │   ├── logger.hpp          # Logger class (uses LoggingConfig)
│   │   └── logger.cpp          # Logger implementation
│   └── server/
│       └── server_config.hpp   # Server config (aggregates db + cache)
└── multi_file_example.cpp      # Main application
```

This structure mimics a real application where:
- Each module lives in its own directory under `src/`
- Each module defines its own config independently
- Each module has implementation files that use their config internally
- **Modules are unaware of where their config comes from in the hierarchy**
- The main application config lives in `config/`
- Config aggregation follows the module dependency hierarchy

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
// database.hpp - Module only knows about its own config
class Database {
public:
    explicit Database(const DatabaseConfig& config);
    void connect();
    void query(const std::string& sql);
private:
    const DatabaseConfig& config_;  // Stores reference, unaware of hierarchy
};

// database.cpp - Uses config internally
void Database::connect() {
    std::cout << config_.username.value << "@"
              << config_.host.value << ":" 
              << config_.port.value << "\n";
}

// main.cpp - Passes nested config to module
Database primary_db(loader.configs.server.primary_db);
primary_db.connect();  // Module uses config internally
```

## Key Benefits

1. **Separation of Concerns**: Each module defines its own config independently
2. **Reusability**: DatabaseConfig used for both primary_db and replica_db
3. **Type Safety**: Compile-time checking of all config access
4. **Zero Overhead**: Direct member access via inheritance
5. **Module Agnostic**: Modules don't need to know about the full hierarchy
6. **Clean Organization**: Config structure mirrors application architecture
