#include "cache.hpp"
#include <iostream>

Cache::Cache(const CacheConfig& config) : config_(config) {}

void Cache::connect() {
    std::cout << "Connecting to cache: " 
              << config_.host.value << ":" 
              << config_.port.value 
              << " (TTL: " << config_.ttl.value << "s)\n";
}

void Cache::set(const std::string& key, const std::string& value) {
    std::cout << "Cache SET " << key << " = " << value 
              << " (expires in " << config_.ttl.value << "s)\n";
}

std::string Cache::get(const std::string& key) {
    std::cout << "Cache GET " << key << " from " << config_.host.value << "\n";
    return "cached_value";
}
