#pragma once
#include "configs_loader.hpp"
#include "connection_pool_config.hpp"

struct DatabaseConfig {
    Config<std::string> host{
        .default_value = "localhost",
        .flags = {"--host"},
        .description = "Database hostname"
    };
    
    Config<int> port{
        .default_value = 5432,
        .flags = {"--port"},
        .description = "Database port"
    };
    
    CONFIG_GROUP(ConnectionPoolConfig, pool);

    REGISTER_CONFIG_FIELDS(host, port, pool)
};
