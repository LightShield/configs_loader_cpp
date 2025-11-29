#include "serialization/preset_deserializer.hpp"
#include "serialization/toml_deserializer.hpp"
#include <filesystem>

std::unique_ptr<PresetDeserializer> create_preset_deserializer(const std::string& file_path) {
    std::filesystem::path path(file_path);
    std::string ext = path.extension().string();
    
    if (ext == ".toml") {
#ifdef CONFIGS_LOADER_ENABLE_TOML
        return std::make_unique<TomlDeserializer>();
#else
        throw std::runtime_error("TOML support not enabled. Recompile with -DENABLE_TOML_PRESETS=ON");
#endif
    }
    
    throw std::runtime_error("Unsupported preset file format: " + ext);
}
