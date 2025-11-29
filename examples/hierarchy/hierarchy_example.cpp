#include "configs_loader.hpp"
#include <iostream>

struct DatabaseConfig {
    Config<std::string> host{
        .default_value = "localhost",
        .flags = {"--host"},
        .description = "Database hostname"
    };
    
    Config<int> port{
        .default_value = 5432,
        .flags = {"--port"},
        .description = "Database port"
    };

    REGISTER_CONFIG_FIELDS(host, port)
};

struct ServerConfig {
    CONFIG_GROUP(DatabaseConfig, primary_db);
    CONFIG_GROUP(DatabaseConfig, replica_db);
    
    Config<int> timeout{
        .default_value = 30,
        .flags = {"--timeout"},
        .description = "Server timeout in seconds"
    };

    REGISTER_CONFIG_FIELDS(primary_db, replica_db, timeout)
};

struct AppConfig {
    CONFIG_GROUP(ServerConfig, backend);
    
    Config<std::string> app_name{
        .default_value = "myapp",
        .flags = {"--name"},
        .description = "Application name"
    };

    REGISTER_CONFIG_FIELDS(backend, app_name)
};

int main(int argc, char* argv[]) {
    ConfigsLoader<AppConfig> loader;
    
    try {
        loader.init(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Configuration error: " << e.what() << "\n";
        return 1;
    }
    
    // Access nested configs with direct member access - zero overhead
    std::cout << "App Name: " << loader.configs.app_name.value << "\n";
    std::cout << "Backend Timeout: " << loader.configs.backend.config.timeout.value << "\n";
    std::cout << "Primary DB Host: " << loader.configs.backend.config.primary_db.config.host.value << "\n";
    std::cout << "Primary DB Port: " << loader.configs.backend.config.primary_db.config.port.value << "\n";
    std::cout << "Replica DB Host: " << loader.configs.backend.config.replica_db.config.host.value << "\n";
    std::cout << "Replica DB Port: " << loader.configs.backend.config.replica_db.config.port.value << "\n";
    
    // Agnostic pattern: use references to eliminate repetitive access
    const auto& primary = loader.configs.backend.config.primary_db;
    const auto& replica = loader.configs.backend.config.replica_db;
    std::cout << "\nUsing references:\n";
    std::cout << "Primary: " << primary.config.host.value << ":" << primary.config.port.value << "\n";
    std::cout << "Replica: " << replica.config.host.value << ":" << replica.config.port.value << "\n";
    
    return 0;
}
