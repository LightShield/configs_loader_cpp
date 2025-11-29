#pragma once
#include "configs_loader.hpp"
#include "../database/database_config.hpp"
#include "../cache/cache_config.hpp"

struct ServerConfig {
    CONFIG_GROUP(DatabaseConfig, primary_db);
    CONFIG_GROUP(DatabaseConfig, replica_db);
    CONFIG_GROUP(CacheConfig, cache);
    
    Config<int> port{
        .default_value = 8080,
        .flags = {"--port"},
        .description = "Server listening port"
    };
    
    Config<int> timeout{
        .default_value = 30,
        .flags = {"--timeout"},
        .description = "Request timeout in seconds"
    };

    REGISTER_CONFIG_FIELDS(primary_db, replica_db, cache, port, timeout)
};
