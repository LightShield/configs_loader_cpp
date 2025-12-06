#pragma once
#include "configs_loader.hpp"

using namespace lightshield::config;

struct LoggingConfig {
    Config<int> level{
        .default_value = 2,
        .flags = {"--level", "-l"},
        .description = "Log level (0=trace, 1=debug, 2=info, 3=warn, 4=error)"
    };
    
    Config<std::string> output{
        .default_value = "stdout",
        .flags = {"--output", "-o"},
        .description = "Log output destination"
    };
    
    Config<bool> timestamps{
        .default_value = true,
        .flags = {"--timestamps"},
        .description = "Include timestamps in logs"
    };

    REGISTER_CONFIG_FIELDS(level, output, timestamps)
};
