#include "configs_loader.hpp"
#include <iostream>

struct MyConfigs {
    Config<std::string> filename{
        .default_value = "input.txt",
        .flags = {"--file", "-f"},
        .required = true,
        .description = "Input file to process"
    };
    
    Config<int> log_level{
        .default_value = 2,
        .flags = {"--log-level", "-l"},
        .description = "Logging verbosity level (0-5)"
    };

    REGISTER_CONFIG_FIELDS(filename, log_level)
};

int main() {
    // Demonstrate with hardcoded arguments
    const char* argv[] = {"basic_example", "--file", "data.txt", "--log-level", "4"};
    int argc = 5;
    
    ConfigsLoader<MyConfigs> loader;
    
    try {
        loader.Init(argc, const_cast<char**>(argv));
    } catch (const std::exception& e) {
        std::cerr << "Configuration error: " << e.what() << "\n";
        return 1;
    }
    
    // Access config values directly - no getter overhead
    std::cout << "Filename: " << loader.configs.filename.value << "\n";
    std::cout << "Log Level: " << loader.configs.log_level.value << "\n";
    
    return 0;
}
