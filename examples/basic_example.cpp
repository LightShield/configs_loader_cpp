#include "configs_loader.hpp"
#include <iostream>

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
    try {
        ConfigsLoader<MyConfigs> loader(argc, argv);
        
        std::cout << "Filename: " << loader.configs.filename.value << "\n";
        std::cout << "Log Level: " << loader.configs.log_level.value << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
