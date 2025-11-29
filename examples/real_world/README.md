# Real World Application Example

Demonstrates a realistic application structure with proper include/src separation, module composition, and sub-module config passing.

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
└── server (ServerConfig)
    ├── database (DatabaseConfig)
    │   ├── host
    │   ├── port
    │   └── pool (ConnectionPoolConfig)  # Sub-module
    │       ├── min_connections
    │       └── max_connections
    ├── cache (CacheConfig)
    │   ├── host
    │   └── port
    └── port
```

## Key Patterns

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

### Module Composition
Server owns Database and Cache, passing their configs:
```cpp
class Server {
    Server(const ServerConfig& config) 
        : db_(config.database)    // Pass sub-config
        , cache_(config.cache)    // Pass sub-config
    {}
private:
    Database db_;
    Cache cache_;
};
```

### Config Access (4 levels deep)
```cpp
loader.configs.server.database.pool.min_connections.value
```

## Usage

```bash
./real_world_example \
  --name production-app \
  --server.port 9000 \
  --server.database.host db.prod.com \
  --server.database.pool.min 10 \
  --server.database.pool.max 50 \
  --server.cache.host cache.prod.com
```

## Benefits

1. **Proper separation**: include/ for headers, src/ for implementation
2. **Sub-modules**: Database has connection pool sub-config
3. **Composition**: Server composes Database and Cache
4. **Config passing**: Modules pass sub-configs to their dependencies
5. **Hierarchy agnostic**: Each module only knows its own config
6. **Real structure**: Mimics actual C++ project organization
