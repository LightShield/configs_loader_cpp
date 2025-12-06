#include "global_configs.hpp"
#include <iostream>

void some_function() {
    // Access configs from any file - no need to pass them around
    std::cout << "Other file - Filename: " << GetConfigs().filename.value << "\n";
    std::cout << "Other file - Log Level: " << GetConfigs().log_level.value << "\n";
}
