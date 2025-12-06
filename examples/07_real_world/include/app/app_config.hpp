#pragma once
#include "configs_loader.hpp"
#include "server/server_config.hpp"

using namespace lightshield::config;

struct AppConfig {
    // Two servers with different default ports (compile-time via designated initializers)
    ConfigGroup<ServerConfig> api_server{
        .config = {
            .port = {.default_value = 8080}
        },
        .name_ = "api_server"
    };
    
    ConfigGroup<ServerConfig> admin_server{
        .config = {
            .port = {.default_value = 9090}
        },
        .name_ = "admin_server"
    };
    
    Config<std::string> name{
        .default_value = "myapp",
        .flags = {"--name"},
        .description = "Application name"
    };

    REGISTER_CONFIG_FIELDS(api_server, admin_server, name)
};
