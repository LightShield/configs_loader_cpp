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
    
    // Use references to simplify access to nested configs
    const auto& api_cfg = loader.configs.api_server.config;
    const auto& admin_cfg = loader.configs.admin_server.config;
    
    // Two servers, each with three caches
    Server api_server(api_cfg);
    std::cout << "API Server:\n";
    api_server.start();
    
    std::cout << "\n";
    
    Server admin_server(admin_cfg);
    std::cout << "Admin Server:\n";
    admin_server.start();
    
    std::cout << "\n=== Config Hierarchy Demo ===\n";
    
    // Using reference aliases for cleaner access
    std::cout << "API server caches (3 instances with different ports):\n";
    std::cout << "  session_cache: " << api_cfg.session_cache.config.port.value << "\n";
    std::cout << "  data_cache: " << api_cfg.data_cache.config.port.value << "\n";
    std::cout << "  query_cache: " << api_cfg.query_cache.config.port.value << "\n";
    
    // Full path access (also valid, but more verbose)
    std::cout << "\nDatabase pool config (4 levels deep - full path):\n";
    std::cout << "  app.api_server.database.pool.min = " 
              << loader.configs.api_server.config.database.config.pool.config.min_connections.value << "\n";
    std::cout << "  app.api_server.database.pool.max = " 
              << loader.configs.api_server.config.database.config.pool.config.max_connections.value << "\n";
    
    return 0;
}
