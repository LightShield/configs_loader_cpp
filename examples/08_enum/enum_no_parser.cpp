#include "configs_loader.hpp"
#include <iostream>

using namespace lightshield::config;

enum class Status : uint8_t {
    Idle = 0,
    Running,
    Stopped
};

struct AppConfig {
    Config<Status> status{
        .default_value = Status::Idle,
        // No parser provided
        .flags = {"--status"},
        .description = "Application status"
    };

    REGISTER_CONFIG_FIELDS(status)
};

int main(int argc, char* argv[]) {
    ConfigsLoader<AppConfig> loader;
    loader.unknown_flag_behavior = UnknownFlagBehavior::Warn;
    loader.init(argc, argv);
    
    std::cout << "Status value: " << static_cast<int>(loader.configs.status.value) << "\n";
    
    return 0;
}
