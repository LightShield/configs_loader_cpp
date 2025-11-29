#include "cache/cache.hpp"
#include <iostream>

Cache::Cache(const CacheConfig& config) : config_(config) {}

void Cache::connect() {
    std::cout << "Connecting to cache: " << config_.host.value << ":" << config_.port.value << "\n";
}
