#include "configs_loader.h"
#include <iostream>

struct MyConfigs {
    ConfigField<std::string> filename{"input.txt", {"--file", "-f"}, true};
    ConfigField<std::string> preset{"", {"--preset", "-p"}};
    ConfigField<int> log_level{2, {"--log-level", "-l"}};

    REGISTER_CONFIG_FIELDS(filename, preset, log_level)
};

int main(int argc, char* argv[]) {
    try {
        ConfigsLoader<MyConfigs> loader(argc, argv);
        
        std::cout << "Filename: " << loader.configs.filename.value() << "\n";
        std::cout << "Log Level: " << loader.configs.log_level.value() << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
