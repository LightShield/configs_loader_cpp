#include "server/server.hpp"
#include <iostream>

// Server has its own database, but shares cache with other servers
Server::Server(const ServerConfig& config, const CacheConfig& cache_config) 
    : config_(config)
    , db_(config.database)      // Each server has its own database config
    , cache_(cache_config)      // All servers share the same cache config
{}

void Server::start() {
    std::cout << "Starting server on port " << config_.port.value << "\n";
    db_.connect();
    cache_.connect();
    std::cout << "Server ready!\n";
}
