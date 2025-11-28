#pragma once

#include "configs_loader.hpp"

// Define your config structure
struct AppConfigs {
    Config<std::string> filename{
        .default_value = "input.txt",
        .flags = {"--file", "-f"},
        .required = true
    };
    
    Config<int> log_level{
        .default_value = 2,
        .flags = {"--log-level", "-l"}
    };

    REGISTER_CONFIG_FIELDS(filename, log_level)
};

// Declare global loader - defined in main.cpp
extern ConfigsLoader<AppConfigs> g_config_loader;

// Convenience accessor
inline AppConfigs& GetConfigs() {
    return g_config_loader.configs;
}
