#pragma once
#include "configs_loader.hpp"
#include "database/database_config.hpp"

struct ServerConfig {
    CONFIG_GROUP(DatabaseConfig, database);  // Each server has its own database
    
    Config<int> port{
        .default_value = 8080,
        .flags = {"--port"},
        .description = "Server port"
    };

    REGISTER_CONFIG_FIELDS(database, port)
};
