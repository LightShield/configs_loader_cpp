#pragma once
#include "server_config.hpp"
#include "database/database.hpp"
#include "cache/cache.hpp"

class Server {
public:
    // Server has its own database config, but shares cache config with other servers
    Server(const ServerConfig& config, const CacheConfig& cache_config);
    void start();

private:
    const ServerConfig& config_;
    Database db_;
    Cache cache_;
};
