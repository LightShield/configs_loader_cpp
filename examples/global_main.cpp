#include "global_configs.hpp"
#include <iostream>

// Define the global config loader
ConfigsLoader<AppConfigs> g_config_loader;

// Example function in another file that uses configs
void some_function();

int main(int argc, char* argv[]) {
    try {
        g_config_loader.Init(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Configuration error: " << e.what() << "\n";
        return 1;
    }
    
    // Use configs in main
    std::cout << "Main - Filename: " << GetConfigs().filename.value << "\n";
    std::cout << "Main - Log Level: " << GetConfigs().log_level.value << "\n";
    
    // Call function that uses configs
    some_function();
    
    return 0;
}
