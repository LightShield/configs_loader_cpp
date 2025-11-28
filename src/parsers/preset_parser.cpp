#include "parsers/preset_parser.hpp"
#include "parsers/toml_parser.hpp"
#include <filesystem>

std::unique_ptr<PresetParser> create_preset_parser(const std::string& file_path) {
    std::filesystem::path path(file_path);
    std::string ext = path.extension().string();
    
    if (ext == ".toml") {
#ifdef CONFIGS_LOADER_ENABLE_TOML
        return std::make_unique<TomlParser>();
#else
        throw std::runtime_error("TOML support not enabled. Recompile with -DENABLE_TOML_PRESETS=ON");
#endif
    }
    
    throw std::runtime_error("Unsupported preset file format: " + ext);
}
