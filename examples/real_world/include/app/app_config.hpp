#pragma once
#include "configs_loader.hpp"
#include "server/server_config.hpp"

struct AppConfig {
    CONFIG_GROUP(ServerConfig, server);
    
    Config<std::string> name{
        .default_value = "myapp",
        .flags = {"--name"},
        .description = "Application name"
    };

    REGISTER_CONFIG_FIELDS(server, name)
};
