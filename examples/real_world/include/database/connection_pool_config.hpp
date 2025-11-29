#pragma once
#include "configs_loader.hpp"

struct ConnectionPoolConfig {
    Config<int> min_connections{
        .default_value = 5,
        .flags = {"--min"},
        .description = "Minimum pool connections"
    };
    
    Config<int> max_connections{
        .default_value = 20,
        .flags = {"--max"},
        .description = "Maximum pool connections"
    };

    REGISTER_CONFIG_FIELDS(min_connections, max_connections)
};
