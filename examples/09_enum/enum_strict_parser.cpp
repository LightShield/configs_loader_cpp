#include "configs_loader.hpp"
#include <iostream>
#include <stdexcept>

using namespace lightshield::config;

enum class LogLevel : uint8_t {
    Error = 0,
    Warn,
    Info,
    Debug
};

const char* log_level_to_string(LogLevel level) noexcept {
    switch (level) {
        case LogLevel::Error: return "error";
        case LogLevel::Warn: return "warn";
        case LogLevel::Info: return "info";
        case LogLevel::Debug: return "debug";
    }
    return "unknown";
}

LogLevel log_level_from_string_strict(const std::string& str) {
    if (str == "error") return LogLevel::Error;
    if (str == "warn") return LogLevel::Warn;
    if (str == "info") return LogLevel::Info;
    if (str == "debug") return LogLevel::Debug;
    throw std::invalid_argument("Invalid log level: " + str + " (valid: error, warn, info, debug)");
}

struct AppConfig {
    Config<LogLevel> log_level{
        .default_value = LogLevel::Info,
        .enum_traits = {
            .parser = log_level_from_string_strict,
            .to_string = [](const LogLevel& level) { return std::string(log_level_to_string(level)); }
        },
        .flags = {"--log-level", "-l"},
        .description = "Logging verbosity (error, warn, info, debug)"
    };

    REGISTER_CONFIG_FIELDS(log_level)
};

int main(int argc, char* argv[]) {
    try {
        ConfigsLoader<AppConfig> loader(argc, argv);
        std::cout << "Log Level: " << log_level_to_string(loader.configs.log_level.value) << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
