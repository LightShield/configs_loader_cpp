#pragma once
#include "configs_loader.hpp"

using namespace lightshield::config;

struct CacheConfig {
    Config<std::string> host{
        .default_value = "localhost",
        .flags = {"--host"},
        .description = "Cache hostname"
    };
    
    Config<int> port{
        .default_value = 6379,
        .flags = {"--port"},
        .description = "Cache port"
    };

    REGISTER_CONFIG_FIELDS(host, port)
};
