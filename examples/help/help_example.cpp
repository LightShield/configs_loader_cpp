#include "configs_loader.hpp"
#include <iostream>

struct AppConfigs {
    Config<std::string> database{
        .default_value = "localhost:5432",
        .flags = {"--database", "-d"},
        .required = true,
        .description = "Database connection string"
    };
    
    Config<int> timeout{
        .default_value = 30,
        .flags = {"--timeout", "-t"},
        .description = "Connection timeout in seconds"
    };
    
    Config<int> retries{
        .default_value = 3,
        .flags = {"--retries", "-r"},
        .description = "Number of retry attempts"
    };

    REGISTER_CONFIG_FIELDS(database, timeout, retries)
};

int main() {
    ConfigsLoader<AppConfigs> loader;
    
    std::cout << "=== Help Generation Example ===\n\n";
    
    // Generate help with default width (80 chars)
    std::cout << "Default width (80 chars):\n";
    std::cout << loader.generate_help("myapp") << "\n";
    
    std::cout << "\n" << std::string(60, '=') << "\n\n";
    
    // Generate help with custom width (60 chars)
    std::cout << "Narrow width (60 chars):\n";
    loader.help_config.max_width = 60;
    std::cout << loader.generate_help("myapp") << "\n";
    
    std::cout << "\n" << std::string(60, '=') << "\n\n";
    
    // Generate help with wide width (120 chars)
    std::cout << "Wide width (120 chars):\n";
    loader.help_config.max_width = 120;
    std::cout << loader.generate_help("myapp") << "\n";
    
    std::cout << "\n" << std::string(60, '=') << "\n\n";
    
    // Generate help without colors
    std::cout << "Without colors:\n";
    loader.help_config.use_colors = false;
    std::cout << loader.generate_help("myapp") << "\n";
    
    return 0;
}
