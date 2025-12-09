#include "configs_loader.hpp"
#include <iostream>

using namespace lightshield::config;

enum class LogLevel : uint8_t {
    Error = 0,
    Warn,
    Info,
    Debug
};

constexpr const char* log_level_to_string(LogLevel level) noexcept {
    switch (level) {
        case LogLevel::Error: return "error";
        case LogLevel::Warn: return "warn";
        case LogLevel::Info: return "info";
        case LogLevel::Debug: return "debug";
    }
    return "unknown";
}

LogLevel log_level_from_string(const std::string& str) noexcept {
    if (str == "error") return LogLevel::Error;
    if (str == "warn") return LogLevel::Warn;
    if (str == "info") return LogLevel::Info;
    if (str == "debug") return LogLevel::Debug;
    return LogLevel::Info;
}

struct AppConfig {
    Config<LogLevel> log_level{
        .default_value = LogLevel::Info,
        .enum_traits = {
            .parser = log_level_from_string,
            .to_string = [](const LogLevel& level) { return std::string(log_level_to_string(level)); }
        },
        .flags = {"--log-level", "-l"},
        .description = "Logging verbosity (error, warn, info, debug)"
    };
    
    Config<std::string> output{
        .default_value = "output.txt",
        .flags = {"--output", "-o"},
        .description = "Output file path"
    };

    REGISTER_CONFIG_FIELDS(log_level, output)
};

int main(int argc, char* argv[]) {
    ConfigsLoader<AppConfig> loader(argc, argv);
    
    std::cout << "Log Level: " << log_level_to_string(loader.configs.log_level.value) << "\n";
    std::cout << "Output: " << loader.configs.output.value << "\n";
    
    std::cout << "\nDumped config (TOML):\n" << loader.dump_configs(SerializationFormat::TOML) << "\n";
    
    return 0;
}
