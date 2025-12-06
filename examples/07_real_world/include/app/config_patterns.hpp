#pragma once
#include "cache/cache_config.hpp"
#include <string>

// Pattern 1: Hold config reference (most flexible)
class CachePattern1 {
public:
    explicit CachePattern1(const CacheConfig& config) : config_(config) {}
    
    void connect() {
        // Access config each time - always up-to-date if config changes
        std::cout << "Pattern 1 - Cache: " << config_.host.value 
                  << ":" << config_.port.value << "\n";
    }

private:
    const CacheConfig& config_;  // Reference to config
};

// Pattern 2: Copy values during construction (snapshot)
class CachePattern2 {
public:
    explicit CachePattern2(const CacheConfig& config) 
        : host_(config.host.value)
        , port_(config.port.value) 
    {}
    
    void connect() {
        // Uses copied values - won't see config changes
        std::cout << "Pattern 2 - Cache: " << host_ << ":" << port_ << "\n";
    }

private:
    std::string host_;  // Copied value
    int port_;          // Copied value
};

// Pattern 3: Hold references to individual config values (reactive)
class CachePattern3 {
public:
    explicit CachePattern3(const CacheConfig& config) 
        : host_(config.host.value)
        , port_(config.port.value)
    {}
    
    void connect() {
        // References to values - sees changes if config is modified
        std::cout << "Pattern 3 - Cache: " << host_ << ":" << port_ << "\n";
    }

private:
    const std::string& host_;  // Reference to value
    const int& port_;          // Reference to value
};
