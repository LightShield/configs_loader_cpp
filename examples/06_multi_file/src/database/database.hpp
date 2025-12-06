#pragma once
#include "database_config.hpp"
#include <string>

using namespace lightshield::config;

// Database module - only knows about DatabaseConfig, not where it comes from
class Database {
public:
    explicit Database(const DatabaseConfig& config);
    
    void connect();
    void query(const std::string& sql);

private:
    const DatabaseConfig& config_;
};
