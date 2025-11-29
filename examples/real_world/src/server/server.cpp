#include "server/server.hpp"
#include <iostream>

Server::Server(const ServerConfig& config) 
    : config_(config)
    , db_(config.database)      // Pass database sub-config
    , cache_(config.cache)      // Pass cache sub-config
{}

void Server::start() {
    std::cout << "Starting server on port " << config_.port.value << "\n";
    db_.connect();
    cache_.connect();
    std::cout << "Server ready!\n";
}
