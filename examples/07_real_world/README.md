# Real World Application Example

Demonstrates advanced patterns: proper include/src separation, sub-module configs, multiple instances with different compile-time defaults, and three config passing strategies.

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
├── api_server (ServerConfig)           # Port 8080 (compile-time default)
│   ├── database (DatabaseConfig)
│   │   └── pool (ConnectionPoolConfig)  # Sub-module
│   │       ├── min_connections
│   │       └── max_connections
│   ├── session_cache (CacheConfig)     # Port 6379
│   ├── data_cache (CacheConfig)        # Port 6380
│   └── query_cache (CacheConfig)       # Port 6381
└── admin_server (ServerConfig)         # Port 9090 (compile-time default)
    ├── database (DatabaseConfig)
    │   └── pool (ConnectionPoolConfig)
    │       ├── min_connections
    │       └── max_connections
    ├── session_cache (CacheConfig)     # Port 6379
    ├── data_cache (CacheConfig)        # Port 6380
    └── query_cache (CacheConfig)       # Port 6381
```

## Advanced Patterns

### 1. Multiple Instances with Different Compile-Time Defaults

Using designated initializers to override nested defaults:

```cpp
struct AppConfig {
    ConfigGroup<ServerConfig> api_server{
        .config = {
            .port = {.default_value = 8080},  // Override just the port
            .session_cache = {.config = {.port = {.default_value = 6379}}},
            .data_cache = {.config = {.port = {.default_value = 6380}}},
            .query_cache = {.config = {.port = {.default_value = 6381}}}
        },
        .name_ = "api_server"
    };
    
    ConfigGroup<ServerConfig> admin_server{
        .config = {
            .port = {.default_value = 9090},  // Different port
            .session_cache = {.config = {.port = {.default_value = 6379}}},
            .data_cache = {.config = {.port = {.default_value = 6380}}},
            .query_cache = {.config = {.port = {.default_value = 6381}}}
        },
        .name_ = "admin_server"
    };
};
```

**Key benefit**: Fully compile-time initialization, no constructor overhead, can override any nested field.

### 2. Sub-Module Config

Database has a connection pool sub-module with its own config:

```cpp
struct DatabaseConfig {
    Config<std::string> host{...};
    Config<int> port{...};
    CONFIG_GROUP(ConnectionPoolConfig, pool);  // Sub-module
    REGISTER_CONFIG_FIELDS(host, port, pool)
};
```

Access: `loader.configs.api_server.config.database.config.pool.config.min_connections.value`

### 3. Three Config Passing Patterns

**Pattern 1: Hold config reference (Database)**
```cpp
class Database {
    const DatabaseConfig& config_;  // Reference to config
public:
    Database(const DatabaseConfig& config) : config_(config) {}
    void connect() {
        // Access config each time - always up-to-date (reactive)
        std::cout << config_.host.value << ":" << config_.port.value;
    }
};
```

**Pattern 2: Copy values during construction (Cache)**
```cpp
class Cache {
    std::string host_;  // Copied value
    int port_;          // Copied value
public:
    Cache(const CacheConfig& config) 
        : host_(config.host.value)
        , port_(config.port.value) 
    {}
    void connect() {
        // Uses snapshot - won't see config changes (immutable)
        std::cout << host_ << ":" << port_;
    }
};
```

**Pattern 3: Hold references to individual values (Server)**
```cpp
class Server {
    const int& port_;  // Reference to value
public:
    Server(const ServerConfig& config) 
        : port_(config.port.value)
    {}
    void start() {
        // Sees changes if config.port.value is modified (reactive)
        std::cout << "Port: " << port_;
    }
};
```

**When to use each:**
- Pattern 1: Need full config access, want reactivity
- Pattern 2: Only need few values, want snapshot/immutability
- Pattern 3: Need reactivity but only for specific values

### 4. Access Patterns for Deep Hierarchies

**Direct access (4 levels deep):**
```cpp
loader.configs.api_server.config.database.config.pool.config.min_connections.value
```

**Scoped alias for cleaner access:**
```cpp
const auto& api_cfg = loader.configs.api_server.config;
std::cout << api_cfg.database.config.pool.config.min_connections.value;

// Or multiple levels
const auto& pool = api_cfg.database.config.pool.config;
std::cout << pool.min_connections.value;
```

## Usage

```bash
# Configure two servers with different databases and multiple caches
./real_world_example \
  --name production-app \
  --api_server.port 8080 \
  --api_server.database.host api-db.prod.com \
  --api_server.database.pool.min 10 \
  --api_server.database.pool.max 50 \
  --api_server.session_cache.host cache1.prod.com \
  --api_server.data_cache.host cache2.prod.com \
  --admin_server.port 9090 \
  --admin_server.database.host admin-db.prod.com
```

## Design Philosophy

### Compile-Time First
- Type safety: All config access is type-checked at compile time
- Direct memory access: Resolves to fixed memory offsets
- IDE support: Full autocomplete and type information
- Designated initializers: Different defaults set at compile time (C++20)

### Developer Experience
- Hierarchical organization: Configs mirror application architecture
- Context awareness: Modules only know their own config, not hierarchy
- Automatic features: Help generation, required field validation, type conversion

### Runtime Efficiency
- Direct access: All member access is compile-time resolved
- Minimal overhead: Composition adds one pointer per ConfigGroup
- Thread safety: Wrap config types with `std::atomic<T>` if needed
