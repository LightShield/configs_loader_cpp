#include "configs_loader.hpp"
#include <iostream>

using namespace lightshield::config;

struct AppConfig {
    Config<std::string> input{
        .default_value = "input.txt",
        .flags = {"--input", "-i"},
        .description = "Input file"
    };

    REGISTER_CONFIG_FIELDS(input)
};

int main(int argc, char* argv[]) {
    std::cout << "=== Error Behavior (default) ===\n";
    {
        ConfigsLoader<AppConfig> loader;
        // Default: UnknownFlagBehavior::Error
        int result = loader.init(argc, argv);
        if (result != 0) {
            std::cout << "Init failed (expected for unknown flags)\n\n";
        } else {
            std::cout << "Input: " << loader.configs.input.value << "\n\n";
        }
    }
    
    std::cout << "=== Warn Behavior ===\n";
    {
        ConfigsLoader<AppConfig> loader;
        loader.unknown_flag_behavior = UnknownFlagBehavior::Warn;
        loader.init(argc, argv);
        std::cout << "Input: " << loader.configs.input.value << "\n\n";
    }
    
    std::cout << "=== Ignore Behavior ===\n";
    {
        ConfigsLoader<AppConfig> loader;
        loader.unknown_flag_behavior = UnknownFlagBehavior::Ignore;
        loader.init(argc, argv);
        std::cout << "Input: " << loader.configs.input.value << "\n";
    }
    
    return 0;
}
