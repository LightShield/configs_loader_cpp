#pragma once
#include "configs_loader.hpp"
#include "../src/server/server_config.hpp"
#include "../src/logging/logging_config.hpp"

struct AppConfig {
    CONFIG_GROUP(ServerConfig, server);
    CONFIG_GROUP(LoggingConfig, logging);
    
    Config<std::string> app_name{
        .default_value = "myapp",
        .flags = {"--name", "-n"},
        .description = "Application name"
    };
    
    Config<std::string> environment{
        .default_value = "development",
        .flags = {"--env", "-e"},
        .description = "Environment (development, staging, production)"
    };

    REGISTER_CONFIG_FIELDS(server, logging, app_name, environment)
};
