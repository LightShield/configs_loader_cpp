#include "database.hpp"
#include <iostream>

Database::Database(const DatabaseConfig& config) : config_(config) {}

void Database::connect() {
    std::cout << "Connecting to database: " 
              << config_.username.value << "@"
              << config_.host.value << ":" 
              << config_.port.value << "\n";
}

void Database::query(const std::string& sql) {
    std::cout << "Executing query on " << config_.host.value << ": " << sql << "\n";
}
