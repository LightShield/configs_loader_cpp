#include "config/app_config.hpp"
#include "src/database/database.hpp"
#include "src/cache/cache.hpp"
#include "src/logging/logger.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    ConfigsLoader<AppConfig> loader;
    
    try {
        loader.init(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Configuration error: " << e.what() << "\n";
        return 1;
    }
    
    // Access top-level configs
    std::cout << "=== Application Configuration ===\n";
    std::cout << "App: " << loader.configs.app_name.value << "\n";
    std::cout << "Environment: " << loader.configs.environment.value << "\n";
    std::cout << "Server Port: " << loader.configs.server.config.port.value << "\n";
    std::cout << "Server Timeout: " << loader.configs.server.config.timeout.value << "s\n\n";
    
    // Initialize modules - each module only knows about its own config
    // The modules are unaware of where their config comes from in the hierarchy
    std::cout << "=== Module Initialization ===\n";
    Database primary_db(loader.configs.server.config.primary_db.config);
    Database replica_db(loader.configs.server.config.replica_db.config);
    Cache cache(loader.configs.server.config.cache.config);
    Logger logger(loader.configs.logging.config);
    
    // Use modules - they internally access their configs
    primary_db.connect();
    replica_db.connect();
    cache.connect();
    
    std::cout << "\n=== Module Operations ===\n";
    primary_db.query("SELECT * FROM users");
    cache.set("user:123", "John Doe");
    cache.get("user:123");
    logger.log(2, "Application started successfully");
    logger.log(3, "This is a warning message");
    
    return 0;
}
