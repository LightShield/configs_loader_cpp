# Real World Application Example

Demonstrates a realistic application structure with proper include/src separation, module composition, and sub-module config passing.

## Design Philosophy

This example showcases the library's core design principles:

### 1. Compile-Time First
- **Type safety**: All config access is type-checked at compile time
- **Direct memory access**: `config.api_server.port.value` resolves to fixed memory offset
- **IDE support**: Full autocomplete and type information in IDEs
- **Zero runtime overhead**: No virtual functions, no pointer indirection

### 2. Init-Time Flexibility
- **Different defaults**: Constructor sets different defaults for multiple instances
- **One-time cost**: Runs once at startup, not during config usage
- **Future improvement**: C++26 reflection will enable compile-time default customization

### 3. Runtime Efficiency
- **Direct access**: All member access is compile-time resolved
- **Thread safety**: Wrap config types with `std::atomic<T>` if needed
- **No overhead**: Inheritance adds zero cost (no virtual functions)

### 4. Developer Experience
- **Hierarchical organization**: Configs mirror application architecture
- **Context awareness**: Modules only know their own config, not hierarchy
- **Automatic features**: Help generation, required field validation, type conversion
- **Input validation**: Custom verifier functions per field
- **Reusability**: Same config type used multiple times with different defaults

## Structure

```
real_world/
├── include/           # Public headers
│   ├── app/
│   │   └── app_config.hpp
│   ├── server/
│   │   ├── server_config.hpp
│   │   └── server.hpp
│   ├── database/
│   │   ├── database_config.hpp
│   │   ├── connection_pool_config.hpp  # Sub-module config
│   │   └── database.hpp
│   └── cache/
│       ├── cache_config.hpp
│       └── cache.hpp
├── src/               # Implementation files
│   ├── server/
│   │   └── server.cpp
│   ├── database/
│   │   └── database.cpp
│   └── cache/
│       └── cache.cpp
└── main.cpp
```

## Config Hierarchy (4 levels)

```
AppConfig
├── api_server (ServerConfig)           # API server with its own database
│   ├── database (DatabaseConfig)
│   │   ├── host
│   │   ├── port
│   │   └── pool (ConnectionPoolConfig)  # Sub-module
│   │       ├── min_connections
│   │       └── max_connections
│   └── port
├── admin_server (ServerConfig)         # Admin server with different database
│   ├── database (DatabaseConfig)
│   │   ├── host
│   │   ├── port
│   │   └── pool (ConnectionPoolConfig)
│   │       ├── min_connections
│   │       └── max_connections
│   └── port
└── shared_cache (CacheConfig)          # Shared cache for both servers
    ├── host
    └── port
```

## Key Patterns

### Multiple Instances with Different Defaults
Two servers with different default values (init-time customization):
```cpp
struct AppConfig {
    ConfigGroup<ServerConfig> api_server{.name_ = "api_server"};
    ConfigGroup<ServerConfig> admin_server{.name_ = "admin_server"};
    
    // Constructor sets different defaults for each instance
    AppConfig() {
        // API server: port 8080, 10-50 connections
        api_server.port.default_value = 8080;
        api_server.database.pool.min_connections.default_value = 10;
        api_server.database.pool.max_connections.default_value = 50;
        
        // Admin server: port 9090, 5-20 connections
        admin_server.port.default_value = 9090;
        admin_server.database.pool.min_connections.default_value = 5;
        admin_server.database.pool.max_connections.default_value = 20;
    }
};

// Constructor runs once at startup (init-time, not runtime)
// All subsequent access is compile-time resolved direct memory access
```

### Sub-Module Config
Database has a connection pool sub-module with its own config:
```cpp
struct DatabaseConfig {
    Config<std::string> host{...};
    Config<int> port{...};
    CONFIG_GROUP(ConnectionPoolConfig, pool);  // Sub-module
    REGISTER_CONFIG_FIELDS(host, port, pool)
};
```

### Config Passing
Server takes its own config plus shared cache config:
```cpp
class Server {
    Server(const ServerConfig& config, const CacheConfig& cache_config) 
        : db_(config.database)    // Own database config
        , cache_(cache_config)    // Shared cache config
    {}
};

// Usage
Server api_server(loader.configs.api_server, loader.configs.shared_cache);
Server admin_server(loader.configs.admin_server, loader.configs.shared_cache);
```

### Config Access (4 levels deep)
```cpp
loader.configs.api_server.database.pool.min_connections.value
loader.configs.admin_server.database.pool.max_connections.value
```

## Usage

```bash
# Configure two servers with different databases but shared cache
./real_world_example \
  --name production-app \
  --api_server.port 8080 \
  --api_server.database.host api-db.prod.com \
  --api_server.database.pool.min 10 \
  --api_server.database.pool.max 50 \
  --admin_server.port 9090 \
  --admin_server.database.host admin-db.prod.com \
  --admin_server.database.pool.min 5 \
  --admin_server.database.pool.max 20 \
  --shared_cache.host cache.prod.com
```

## Benefits

### Performance
1. **Compile-time resolution**: All config access resolves to direct memory offsets
2. **Zero runtime overhead**: No virtual functions, no pointer indirection
3. **Init-time only**: Constructor runs once at startup, not during usage
4. **Thread-safe option**: Wrap types with `std::atomic<T>` if needed

### Developer Experience
5. **IDE support**: Full autocomplete and type information
6. **Type safety**: Compile-time checking of all config access
7. **Hierarchical organization**: Configs mirror application structure
8. **Context awareness**: Modules only know their own config

### Features
9. **Multiple instances**: Reuse config types with different defaults
10. **Sub-modules**: Nested config hierarchies (database.pool)
11. **Shared configs**: Multiple modules can share same config
12. **Automatic features**: Help, validation, type conversion

### Structure
13. **Proper separation**: include/ for headers, src/ for implementation
14. **Real patterns**: Mimics actual C++ project organization
