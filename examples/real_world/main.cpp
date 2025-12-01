#include "app/app_config.hpp"
#include "server/server.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    ConfigsLoader<AppConfig> loader;
    
    try {
        loader.init(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    std::cout << "=== " << loader.configs.name.value << " ===\n\n";
    
    // Pattern 1: Scoped aliases for cleaner access
    const auto& api_cfg = loader.configs.api_server.config;  // Type deduced
    const ServerConfig& admin_cfg = loader.configs.admin_server.config;  // Explicit type
    
    Server api_server(api_cfg);
    std::cout << "API Server:\n";
    api_server.start();
    
    std::cout << "\n";
    
    Server admin_server(admin_cfg);
    std::cout << "Admin Server:\n";
    admin_server.start();
    
    std::cout << "\n=== Module Config Usage Patterns ===\n";
    std::cout << "Database: Pattern 1 - holds config reference (always up-to-date)\n";
    std::cout << "Cache: Pattern 2 - copies values at construction (snapshot)\n";
    std::cout << "Server: Pattern 3 - holds references to individual values (reactive)\n";
    
    std::cout << "\n=== Config Access Patterns (Global Scope) ===\n";
    
    std::cout << "Admin server caches (using implicit conversion):\n";
    const ServerConfig& admin_server_cfg = loader.configs.admin_server;
    const CacheConfig& session = admin_server_cfg.session_cache;
    const CacheConfig& data = admin_server_cfg.data_cache;
    const CacheConfig& query = admin_server_cfg.query_cache;
    std::cout << "  session_cache: " << session.port.value << "\n";
    std::cout << "  data_cache: " << data.port.value << "\n";
    std::cout << "  query_cache: " << query.port.value << "\n";
    
    std::cout << "\nDatabase pool config (using references):\n";
    const ServerConfig& api_server_cfg = loader.configs.api_server;
    const DatabaseConfig& database = api_server_cfg.database;
    const ConnectionPoolConfig& pool = database.pool;
    std::cout << "  app.api_server.database.pool.min = " << pool.min_connections.value << "\n";
    std::cout << "  app.api_server.database.pool.max = " << pool.max_connections.value << "\n";
    
    return 0;
}
