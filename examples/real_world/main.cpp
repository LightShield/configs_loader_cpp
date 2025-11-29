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
    
    // Two servers, each with three caches
    Server api_server(loader.configs.api_server.config);
    std::cout << "API Server:\n";
    api_server.start();
    
    std::cout << "\n";
    
    Server admin_server(loader.configs.admin_server.config);
    std::cout << "Admin Server:\n";
    admin_server.start();
    
    std::cout << "\n=== Config Hierarchy Demo ===\n";
    std::cout << "API server caches (3 instances with different ports):\n";
    std::cout << "  session_cache: " << loader.configs.api_server.config.session_cache.config.port.value << "\n";
    std::cout << "  data_cache: " << loader.configs.api_server.config.data_cache.config.port.value << "\n";
    std::cout << "  query_cache: " << loader.configs.api_server.config.query_cache.config.port.value << "\n";
    
    std::cout << "\nDatabase pool config (4 levels deep):\n";
    std::cout << "  app.api_server.database.pool.min = " 
              << loader.configs.api_server.config.database.config.pool.config.min_connections.value << "\n";
    std::cout << "  app.api_server.database.pool.max = " 
              << loader.configs.api_server.config.database.config.pool.config.max_connections.value << "\n";
    
    return 0;
}
