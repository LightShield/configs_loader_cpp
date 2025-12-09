#include "configs_loader.hpp"
#include <iostream>

using namespace lightshield::config;

// Plain enum (not enum class)
enum Color {
    Red = 0,
    Green = 1,
    Blue = 2
};

Color color_from_string(const std::string& str) noexcept {
    if (str == "red") return Red;
    if (str == "green") return Green;
    if (str == "blue") return Blue;
    return Red;
}

const char* color_to_string(Color c) noexcept {
    switch (c) {
        case Red: return "red";
        case Green: return "green";
        case Blue: return "blue";
    }
    return "unknown";
}

struct AppConfig {
    Config<Color> color{
        .default_value = Green,
        .enum_traits = {
            .parser = color_from_string,
            .to_string = [](const Color& c) { return std::string(color_to_string(c)); }
        },
        .flags = {"--color"},
        .description = "Color selection"
    };

    REGISTER_CONFIG_FIELDS(color)
};

int main(int argc, char* argv[]) {
    ConfigsLoader<AppConfig> loader(argc, argv);
    
    std::cout << "Color: " << color_to_string(loader.configs.color.value) << "\n";
    std::cout << "\nDumped config:\n" << loader.dump_configs(SerializationFormat::TOML) << "\n";
    
    return 0;
}
