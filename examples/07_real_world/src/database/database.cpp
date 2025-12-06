#include "database/database.hpp"
#include <iostream>

using namespace lightshield::config;

Database::Database(const DatabaseConfig& config) : config_(config) {}

void Database::connect() {
    std::cout << "Connecting to " << config_.host.value << ":" << config_.port.value << "\n";
    const ConnectionPoolConfig& pool = config_.pool;
    std::cout << "  Pool: min=" << pool.min_connections.value 
              << ", max=" << pool.max_connections.value << "\n";
}

void Database::execute(const std::string& query) {
    std::cout << "Executing on " << config_.host.value << ": " << query << "\n";
}
