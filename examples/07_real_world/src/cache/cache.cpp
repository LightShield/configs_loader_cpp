#include "cache/cache.hpp"
#include <iostream>

using namespace lightshield::config;

Cache::Cache(const CacheConfig& config) 
    : host_(config.host.value)
    , port_(config.port.value)
{}

void Cache::connect() {
    std::cout << "Connecting to cache: " << host_ << ":" << port_ << "\n";
}
