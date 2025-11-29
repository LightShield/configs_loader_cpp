#pragma once
#include "server_config.hpp"
#include "database/database.hpp"
#include "cache/cache.hpp"

// Pattern 3: Hold references to individual config values (reactive to changes)
class Server {
public:
    explicit Server(const ServerConfig& config);
    void start();

private:
    const int& port_;  // Reference to value (reactive)
    Database db_;
    Cache session_cache_;
    Cache data_cache_;
    Cache query_cache_;
};
