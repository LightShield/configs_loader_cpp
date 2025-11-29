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
    
    // Pattern 1: Type alias for compile-time type (useful for function parameters)
    using ApiServerConfig = decltype(loader.configs.api_server.config);
    
    // Pattern 2: Reference alias for runtime access (cleaner code)
    const auto& admin_cfg = loader.configs.admin_server.config;
    
    // Two servers, each with three caches
    Server api_server(loader.configs.api_server.config);
    std::cout << "API Server:\n";
    api_server.start();
    
    std::cout << "\n";
    
    Server admin_server(admin_cfg);
    std::cout << "Admin Server:\n";
    admin_server.start();
    
    std::cout << "\n=== Config Hierarchy Demo ===\n";
    
    // Using reference alias for cleaner access
    std::cout << "Admin server caches (using reference alias):\n";
    std::cout << "  session_cache: " << admin_cfg.session_cache.config.port.value << "\n";
    std::cout << "  data_cache: " << admin_cfg.data_cache.config.port.value << "\n";
    std::cout << "  query_cache: " << admin_cfg.query_cache.config.port.value << "\n";
    
    // Full path access (also valid, but more verbose)
    std::cout << "\nDatabase pool config (4 levels deep - full path):\n";
    std::cout << "  app.api_server.database.pool.min = " 
              << loader.configs.api_server.config.database.config.pool.config.min_connections.value << "\n";
    std::cout << "  app.api_server.database.pool.max = " 
              << loader.configs.api_server.config.database.config.pool.config.max_connections.value << "\n";
    
    return 0;
}
