#pragma once
#include "cache_config.hpp"

class Cache {
public:
    explicit Cache(const CacheConfig& config);
    void connect();

private:
    const CacheConfig& config_;
};
