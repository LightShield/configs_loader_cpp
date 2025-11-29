#pragma once
#include "configs_loader.hpp"

struct DatabaseConfig {
    Config<std::string> host{
        .default_value = "localhost",
        .flags = {"--host", "-h"},
        .description = "Database hostname"
    };
    
    Config<int> port{
        .default_value = 5432,
        .flags = {"--port"},
        .description = "Database port"
    };
    
    Config<std::string> username{
        .default_value = "admin",
        .flags = {"--user", "-u"},
        .description = "Database username"
    };

    REGISTER_CONFIG_FIELDS(host, port, username)
};
