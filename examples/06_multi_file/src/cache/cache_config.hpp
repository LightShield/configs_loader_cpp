#pragma once
#include "configs_loader.hpp"

using namespace lightshield::config;

struct CacheConfig {
    Config<std::string> host{
        .default_value = "localhost",
        .flags = {"--host"},
        .description = "Cache server hostname"
    };
    
    Config<int> port{
        .default_value = 6379,
        .flags = {"--port"},
        .description = "Cache server port"
    };
    
    Config<int> ttl{
        .default_value = 3600,
        .flags = {"--ttl"},
        .description = "Cache TTL in seconds"
    };

    REGISTER_CONFIG_FIELDS(host, port, ttl)
};
