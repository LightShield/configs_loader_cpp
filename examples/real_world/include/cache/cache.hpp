#pragma once
#include "cache_config.hpp"
#include <string>

// Pattern 2: Copy values during construction (snapshot, won't see config changes)
class Cache {
public:
    explicit Cache(const CacheConfig& config);
    void connect();

private:
    std::string host_;  // Copied value
    int port_;          // Copied value
};
