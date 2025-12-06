#include "config/app_config.hpp"
#include "src/database/database.hpp"
#include "src/cache/cache.hpp"
#include "src/logging/logger.hpp"
#include <iostream>

using namespace lightshield::config;

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
    
    const ServerConfig& server = loader.configs.server;
    std::cout << "Server Port: " << server.port.value << "\n";
    std::cout << "Server Timeout: " << server.timeout.value << "s\n\n";
    
    std::cout << "=== Module Initialization ===\n";
    Database primary_db(server.primary_db);
    Database replica_db(server.replica_db);
    Cache cache(server.cache);
    const LoggingConfig& logging = loader.configs.logging;
    Logger logger(logging);
    
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
