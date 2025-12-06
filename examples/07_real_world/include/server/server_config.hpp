#pragma once
#include "configs_loader.hpp"
#include "database/database_config.hpp"
#include "cache/cache_config.hpp"

struct ServerConfig {
    CONFIG_GROUP(DatabaseConfig, database);  // Each server has its own database
    
    // Three caches with different default ports
    ConfigGroup<CacheConfig> session_cache{
        .config = {
            .port = {.default_value = 6379}
        },
        .name_ = "session_cache"
    };
    
    ConfigGroup<CacheConfig> data_cache{
        .config = {
            .port = {.default_value = 6380}
        },
        .name_ = "data_cache"
    };
    
    ConfigGroup<CacheConfig> query_cache{
        .config = {
            .port = {.default_value = 6381}
        },
        .name_ = "query_cache"
    };
    
    Config<int> port{
        .default_value = 8080,
        .flags = {"--port"},
        .description = "Server port"
    };

    REGISTER_CONFIG_FIELDS(database, session_cache, data_cache, query_cache, port)
};
