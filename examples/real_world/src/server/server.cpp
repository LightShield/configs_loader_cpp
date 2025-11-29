#include "server/server.hpp"
#include <iostream>

Server::Server(const ServerConfig& config) 
    : config_(config)
    , db_(config.database.config)
    , session_cache_(config.session_cache.config)
    , data_cache_(config.data_cache.config)
    , query_cache_(config.query_cache.config)
{}

void Server::start() {
    std::cout << "Starting server on port " << config_.port.value << "\n";
    db_.connect();
    session_cache_.connect();
    data_cache_.connect();
    query_cache_.connect();
    std::cout << "Server ready!\n";
}
