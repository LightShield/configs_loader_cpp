#pragma once
#include "logging_config.hpp"
#include <string>

// Logger module - only knows about LoggingConfig, not where it comes from
class Logger {
public:
    explicit Logger(const LoggingConfig& config);
    
    void log(int level, const std::string& message);

private:
    const LoggingConfig& config_;
    std::string level_name(int level) const;
};
