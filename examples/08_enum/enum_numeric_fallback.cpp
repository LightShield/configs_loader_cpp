#include "configs_loader.hpp"
#include <iostream>

using namespace lightshield::config;

enum class Priority : uint8_t {
    Low = 0,
    Medium,
    High,
    Critical
};

Priority priority_from_string(const std::string& str) noexcept {
    if (str == "low") return Priority::Low;
    if (str == "medium") return Priority::Medium;
    if (str == "high") return Priority::High;
    if (str == "critical") return Priority::Critical;
    return Priority::Medium;
}

struct AppConfig {
    // WITH to_string - shows string names
    Config<Priority> priority_with_string{
        .default_value = Priority::Medium,
        .enum_traits = {
            .parser = priority_from_string,
            .to_string = [](const Priority& p) {
                switch (p) {
                    case Priority::Low: return std::string("low");
                    case Priority::Medium: return std::string("medium");
                    case Priority::High: return std::string("high");
                    case Priority::Critical: return std::string("critical");
                }
                return std::string("unknown");
            }
        },
        .flags = {"--priority-named"},
        .description = "Priority level with string names"
    };
    
    // WITHOUT to_string - shows numeric values
    Config<Priority> priority_numeric{
        .default_value = Priority::High,
        .enum_traits = {
            .parser = priority_from_string
        },
        .flags = {"--priority-numeric"},
        .description = "Priority level with numeric fallback"
    };

    REGISTER_CONFIG_FIELDS(priority_with_string, priority_numeric)
};

int main(int argc, char* argv[]) {
    ConfigsLoader<AppConfig> loader(argc, argv);
    
    std::cout << "=== TOML Output ===\n";
    std::cout << loader.dump_configs(SerializationFormat::TOML) << "\n";
    
    std::cout << "=== CLI Output ===\n";
    std::cout << loader.dump_configs(SerializationFormat::CLI) << "\n";
    
    return 0;
}
