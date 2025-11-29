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
    
    std::cout << "\n=== Config Access Patterns For Global Scope===\n";
    
    // Pattern 1: Using scoped aliases (cleaner)
    std::cout << "Admin server caches (using scoped alias):\n";
    std::cout << "  session_cache: " << admin_cfg.session_cache.config.port.value << "\n";
    std::cout << "  data_cache: " << admin_cfg.data_cache.config.port.value << "\n";
    std::cout << "  query_cache: " << admin_cfg.query_cache.config.port.value << "\n";
    
    // Pattern 2: Full global path (explicit, more verbose)
    std::cout << "\nDatabase pool config (using full path):\n";
    std::cout << "  app.api_server.database.pool.min = " 
              << loader.configs.api_server.config.database.config.pool.config.min_connections.value << "\n";
    std::cout << "  app.api_server.database.pool.max = " 
              << loader.configs.api_server.config.database.config.pool.config.max_connections.value << "\n";
    
    return 0;
}
