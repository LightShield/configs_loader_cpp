#include "logger.hpp"
#include <iostream>
#include <ctime>

Logger::Logger(const LoggingConfig& config) : config_(config) {}

void Logger::log(int level, const std::string& message) {
    if (level < config_.level.value) return;
    
    if (config_.timestamps.value) {
        std::time_t now = std::time(nullptr);
        std::cout << "[" << std::ctime(&now) << "] ";
    }
    
    std::cout << "[" << level_name(level) << "] " << message 
              << " (output: " << config_.output.value << ")\n";
}

std::string Logger::level_name(int level) const {
    switch(level) {
        case 0: return "TRACE";
        case 1: return "DEBUG";
        case 2: return "INFO";
        case 3: return "WARN";
        case 4: return "ERROR";
        default: return "UNKNOWN";
    }
}
