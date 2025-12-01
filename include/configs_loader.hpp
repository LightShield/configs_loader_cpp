#pragma once

#include "config.hpp"
#include "help/help_generator.hpp"
#include "serialization/serialization_format.hpp"
#include <string>

// Current macro - requires listing all fields (C++20)
#define REGISTER_CONFIG_FIELDS(...) \
    auto get_fields() { \
        return std::tie(__VA_ARGS__); \
    } \
    auto get_fields() const { \
        return std::tie(__VA_ARGS__); \
    }

// Future macro - will use C++26 reflection to auto-detect fields
// Reserved for future use - DO NOT IMPLEMENT until C++26 is available
#define REGISTER_CONFIG_STRUCT(StructName) \
    static_assert(false, "REGISTER_CONFIG_STRUCT requires C++26 reflection - use REGISTER_CONFIG_FIELDS for now");

template<typename ConfigsType>
class ConfigsLoader {
public:
    ConfigsType configs;
    HelpConfig help_config;

    ConfigsLoader() = default;
    ConfigsLoader(int argc, char* argv[]);

    // Initialize from command-line arguments
    // Automatically handles --help/-h flags (prints help and exits)
    void init(int argc, char* argv[]);

    // Check if init() has been called
    [[nodiscard]] bool is_initialized() const;

    // Generate help text
    // filter: Optional filter for interactive help (e.g., "required", "group_name")
    [[nodiscard]] std::string generate_help(const std::string& program_name = "program", const std::string& filter = "") const;

    // Dump current configuration values
    // format: Output format (see SerializationFormat for details)
    // only_changes: If true, only dump values that differ from defaults
    [[nodiscard]] std::string dump_configs(SerializationFormat format = SerializationFormat::CLI, bool only_changes = false) const;

private:
    bool m_initialized = false;
    
    void load_preset_file(const std::string& path);
};

// Include implementation
#include "configs_loader_impl.hpp"
