#pragma once
#include "database_config.hpp"

class Database {
public:
    explicit Database(const DatabaseConfig& config);
    void connect();
    void execute(const std::string& query);

private:
    const DatabaseConfig& config_;
};
