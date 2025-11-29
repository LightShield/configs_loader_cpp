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
    
    // Server internally creates Database and Cache, passing their configs
    Server server(loader.configs.server);
    server.start();
    
    std::cout << "\n=== Config Hierarchy Demo ===\n";
    std::cout << "Database pool config (3 levels deep):\n";
    std::cout << "  app.server.database.pool.min = " 
              << loader.configs.server.database.pool.min_connections.value << "\n";
    std::cout << "  app.server.database.pool.max = " 
              << loader.configs.server.database.pool.max_connections.value << "\n";
    
    return 0;
}
