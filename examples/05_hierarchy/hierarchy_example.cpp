#include "configs_loader.hpp"
#include <iostream>

using namespace lightshield::config;

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
    
    std::cout << "App Name: " << loader.configs.app_name.value << "\n";
    
    const ServerConfig& backend = loader.configs.backend;
    std::cout << "Backend Timeout: " << backend.timeout.value << "\n";
    
    const DatabaseConfig& primary = backend.primary_db;
    const DatabaseConfig& replica = backend.replica_db;
    std::cout << "Primary DB Host: " << primary.host.value << "\n";
    std::cout << "Primary DB Port: " << primary.port.value << "\n";
    std::cout << "Replica DB Host: " << replica.host.value << "\n";
    std::cout << "Replica DB Port: " << replica.port.value << "\n";
    
    std::cout << "\nUsing implicit conversion:\n";
    std::cout << "Primary: " << primary.host.value << ":" << primary.port.value << "\n";
    std::cout << "Replica: " << replica.host.value << ":" << replica.port.value << "\n";
    
    return 0;
}
