#pragma once
#include "configs_loader.hpp"
#include "database/database_config.hpp"
#include "cache/cache_config.hpp"

struct ServerConfig {
    CONFIG_GROUP(DatabaseConfig, database);
    CONFIG_GROUP(CacheConfig, cache);
    
    Config<int> port{
        .default_value = 8080,
        .flags = {"--port"},
        .description = "Server port"
    };

    REGISTER_CONFIG_FIELDS(database, cache, port)
};
