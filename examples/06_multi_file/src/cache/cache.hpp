#pragma once
#include "cache_config.hpp"
#include <string>

using namespace lightshield::config;

// Cache module - only knows about CacheConfig, not where it comes from
class Cache {
public:
    explicit Cache(const CacheConfig& config);
    
    void connect();
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);

private:
    const CacheConfig& config_;
};
