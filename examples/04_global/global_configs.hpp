#pragma once

#include "configs_loader.hpp"
#include <stdexcept>

using namespace lightshield::config;

// Define your config structure
struct AppConfigs {
    Config<std::string> filename{
        .default_value = "input.txt",
        .flags = {"--file", "-f"},
        .required = true,
        .description = "Input file to process"
    };
    
    Config<int> log_level{
        .default_value = 2,
        .flags = {"--log-level", "-l"},
        .description = "Logging verbosity level (0-5)"
    };

    REGISTER_CONFIG_FIELDS(filename, log_level)
};

// Declare global loader - defined in main.cpp
extern ConfigsLoader<AppConfigs> g_config_loader;

// Safe accessor - checks initialization
inline AppConfigs& GetConfigs() {
    if (!g_config_loader.is_initialized()) {
        throw std::runtime_error("Config loader not initialized - call Init() first");
    }
    return g_config_loader.configs;
}

// Unsafe direct access - no checks, faster but caller must ensure Init() was called
// Use when performance is critical and initialization is guaranteed
// Example: g_config_loader.configs.filename.value
