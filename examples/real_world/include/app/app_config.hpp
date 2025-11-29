#pragma once
#include "configs_loader.hpp"
#include "server/server_config.hpp"
#include "cache/cache_config.hpp"

struct AppConfig {
    // Two servers with different defaults (set in constructor)
    ConfigGroup<ServerConfig> api_server{.name_ = "api_server"};
    ConfigGroup<ServerConfig> admin_server{.name_ = "admin_server"};
    CONFIG_GROUP(CacheConfig, shared_cache);
    
    Config<std::string> name{
        .default_value = "myapp",
        .flags = {"--name"},
        .description = "Application name"
    };
    
    // Constructor sets different defaults for each instance (init-time)
    AppConfig() {
        // API server: port 8080, 10-50 connections
        api_server.port.default_value = 8080;
        api_server.port.value = 8080;
        api_server.database.host.default_value = "api-db.local";
        api_server.database.host.value = "api-db.local";
        api_server.database.pool.min_connections.default_value = 10;
        api_server.database.pool.min_connections.value = 10;
        api_server.database.pool.max_connections.default_value = 50;
        api_server.database.pool.max_connections.value = 50;
        
        // Admin server: port 9090, 5-20 connections
        admin_server.port.default_value = 9090;
        admin_server.port.value = 9090;
        admin_server.database.host.default_value = "admin-db.local";
        admin_server.database.host.value = "admin-db.local";
        admin_server.database.pool.min_connections.default_value = 5;
        admin_server.database.pool.min_connections.value = 5;
        admin_server.database.pool.max_connections.default_value = 20;
        admin_server.database.pool.max_connections.value = 20;
    }

    REGISTER_CONFIG_FIELDS(api_server, admin_server, shared_cache, name)
};
