#pragma once
#include "server_config.hpp"
#include "database/database.hpp"
#include "cache/cache.hpp"

class Server {
public:
    explicit Server(const ServerConfig& config);
    void start();

private:
    const ServerConfig& config_;
    Database db_;
    Cache cache_;
};
