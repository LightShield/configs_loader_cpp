#include "configs_loader.hpp"
#include <iostream>
#include <cstring>

struct MyConfigs {
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

int main(int argc, char* argv[]) {
    ConfigsLoader<MyConfigs> loader;
    
    // Check for help flag
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--help") == 0 || std::strcmp(argv[i], "-h") == 0) {
            std::cout << loader.generate_help(argv[0]) << "\n";
            return 0;
        }
    }
    
    try {
        loader.Init(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Configuration error: " << e.what() << "\n";
        std::cerr << "\nUse --help for usage information\n";
        return 1;
    }
    
    // Usage never throws - safe to use
    std::cout << "Filename: " << loader.configs.filename.value << "\n";
    std::cout << "Log Level: " << loader.configs.log_level.value << "\n";
    
    return 0;
}
