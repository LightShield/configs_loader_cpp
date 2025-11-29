#include "database/database.hpp"
#include <iostream>

Database::Database(const DatabaseConfig& config) : config_(config) {}

void Database::connect() {
    std::cout << "Connecting to " << config_.host.value << ":" << config_.port.value << "\n";
    std::cout << "  Pool: min=" << config_.pool.config.min_connections.value 
              << ", max=" << config_.pool.config.max_connections.value << "\n";
}

void Database::execute(const std::string& query) {
    std::cout << "Executing on " << config_.host.value << ": " << query << "\n";
}
