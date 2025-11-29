#pragma once
#include "configs_loader.hpp"
#include "server/server_config.hpp"
#include "cache/cache_config.hpp"

struct AppConfig {
    // Two servers with different defaults using designated initializers (compile-time)
    ConfigGroup<ServerConfig> api_server{
        .config = {
            .database = {
                .config = {
                    .host = {.default_value = "api-db.local"},
                    .pool = {
                        .config = {
                            .min_connections = {.default_value = 10},
                            .max_connections = {.default_value = 50}
                        }
                    }
                }
            },
            .port = {.default_value = 8080}
        },
        .name_ = "api_server"
    };
    
    ConfigGroup<ServerConfig> admin_server{
        .config = {
            .database = {
                .config = {
                    .host = {.default_value = "admin-db.local"},
                    .pool = {
                        .config = {
                            .min_connections = {.default_value = 5},
                            .max_connections = {.default_value = 20}
                        }
                    }
                }
            },
            .port = {.default_value = 9090}
        },
        .name_ = "admin_server"
    };
    
    CONFIG_GROUP(CacheConfig, shared_cache);
    
    Config<std::string> name{
        .default_value = "myapp",
        .flags = {"--name"},
        .description = "Application name"
    };

    REGISTER_CONFIG_FIELDS(api_server, admin_server, shared_cache, name)
};
