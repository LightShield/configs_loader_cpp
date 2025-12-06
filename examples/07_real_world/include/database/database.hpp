#pragma once
#include "database_config.hpp"

using namespace lightshield::config;

// Pattern 1: Hold config reference (most flexible, always up-to-date)
class Database {
public:
    explicit Database(const DatabaseConfig& config);
    void connect();
    void execute(const std::string& query);

private:
    const DatabaseConfig& config_;  // Reference to config
};
