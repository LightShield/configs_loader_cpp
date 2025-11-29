#include "app_config.hpp"
#include <iostream>

// Simulated database module that only knows about DatabaseConfig
void connect_database(const DatabaseConfig& db, const std::string& name) {
    std::cout << name << " DB: " << db.username.value << "@" 
              << db.host.value << ":" << db.port.value << "\n";
}

// Simulated cache module that only knows about CacheConfig
void connect_cache(const CacheConfig& cache) {
    std::cout << "Cache: " << cache.host.value << ":" << cache.port.value 
              << " (TTL: " << cache.ttl.value << "s)\n";
}

// Simulated logger that only knows about LoggingConfig
void init_logger(const LoggingConfig& log) {
    std::cout << "Logger: level=" << log.level.value 
              << ", output=" << log.output.value 
              << ", timestamps=" << (log.timestamps.value ? "on" : "off") << "\n";
}

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
    std::cout << "Server Port: " << loader.configs.server.port.value << "\n";
    std::cout << "Server Timeout: " << loader.configs.server.timeout.value << "s\n\n";
    
    // Access nested configs directly
    std::cout << "=== Direct Access ===\n";
    std::cout << "Primary DB Host: " << loader.configs.server.primary_db.host.value << "\n";
    std::cout << "Primary DB Port: " << loader.configs.server.primary_db.port.value << "\n";
    std::cout << "Replica DB Host: " << loader.configs.server.replica_db.host.value << "\n";
    std::cout << "Cache Host: " << loader.configs.server.cache.host.value << "\n";
    std::cout << "Log Level: " << loader.configs.logging.level.value << "\n\n";
    
    // Use references for cleaner access
    std::cout << "=== Using References ===\n";
    const auto& server = loader.configs.server;
    std::cout << "Server: " << server.primary_db.host.value << ":" << server.port.value << "\n\n";
    
    // Pass to module-specific functions (agnostic pattern)
    std::cout << "=== Module Initialization ===\n";
    connect_database(loader.configs.server.primary_db, "Primary");
    connect_database(loader.configs.server.replica_db, "Replica");
    connect_cache(loader.configs.server.cache);
    init_logger(loader.configs.logging);
    
    return 0;
}
