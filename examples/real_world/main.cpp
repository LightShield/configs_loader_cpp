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
    
    // Two servers with different database configs but shared cache
    // API server uses api_server.database config
    Server api_server(loader.configs.api_server, loader.configs.shared_cache);
    std::cout << "API Server:\n";
    api_server.start();
    
    std::cout << "\n";
    
    // Admin server uses admin_server.database config (different from API)
    Server admin_server(loader.configs.admin_server, loader.configs.shared_cache);
    std::cout << "Admin Server:\n";
    admin_server.start();
    
    std::cout << "\n=== Config Hierarchy Demo ===\n";
    std::cout << "API server database pool (4 levels deep):\n";
    std::cout << "  app.api_server.database.pool.min = " 
              << loader.configs.api_server.database.pool.min_connections.value << "\n";
    std::cout << "  app.api_server.database.pool.max = " 
              << loader.configs.api_server.database.pool.max_connections.value << "\n";
    
    std::cout << "\nAdmin server database pool (different config):\n";
    std::cout << "  app.admin_server.database.pool.min = " 
              << loader.configs.admin_server.database.pool.min_connections.value << "\n";
    std::cout << "  app.admin_server.database.pool.max = " 
              << loader.configs.admin_server.database.pool.max_connections.value << "\n";
    
    std::cout << "\nShared cache (same for both servers):\n";
    std::cout << "  app.shared_cache.host = " << loader.configs.shared_cache.host.value << "\n";
    std::cout << "  app.shared_cache.port = " << loader.configs.shared_cache.port.value << "\n";
    
    return 0;
}
