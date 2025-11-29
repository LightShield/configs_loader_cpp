#pragma once

#include "config.hpp"
#include <string>

// Forward declarations
class PresetParser;

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

// TODO(C++26): Move preset flag validation to compile-time using reflection
// Currently checked at Init() time due to C++20 limitations

template<typename ConfigsType>
class ConfigsLoader {
public:
    ConfigsType configs;
    
    // Configuration for help output
    struct HelpConfig {
        bool use_colors = true;           // Enable ANSI colors in help output
        bool enable_interactive = true;   // Enable --help <filter> for filtered help
    } help_config;

    ConfigsLoader() = default;
    ConfigsLoader(int argc, char* argv[]);

    // Initialize from command-line arguments
    // Automatically handles --help/-h flags (prints help and exits)
    void init(int argc, char* argv[]);

    // Check if init() has been called
    [[nodiscard]] bool is_initialized() const;

    // Generate help text
    // max_width: Maximum line width for text wrapping (default: 80)
    // filter: Optional filter for interactive help (e.g., "required", "group_name")
    [[nodiscard]] std::string generate_help(const std::string& program_name = "program", size_t max_width = 80, const std::string& filter = "") const;

    // Dump current configuration values
    // only_changes: If true, only dump values that differ from defaults
    [[nodiscard]] std::string dump_configs(bool only_changes = false) const;
    
    // Dump configuration to TOML format
    // only_changes: If true, only dump values that differ from defaults
    [[nodiscard]] std::string dump_to_toml(bool only_changes = false) const;

private:
    bool m_initialized = false;
    
    // Implementation details in configs_loader_impl.hpp
    struct FieldInfo;
    
    std::optional<std::string> extract_preset_path(int argc, char* argv[]);
    void load_preset_file(const std::string& path);
    void parse_cli_arguments(int argc, char* argv[]);
    void validate_required_fields();
    void validate_no_preset_override();
    void try_set_config_value(const std::string& flag, const std::string& value);
    
    // Help generation private helpers (implementation in help/help_generator_impl.hpp)
    std::string generate_help_navigation(const std::string& program_name, bool use_colors) const;
    std::string generate_help_groups(const std::string& program_name, bool use_colors) const;
    std::string generate_help_required(const std::string& program_name, bool use_colors) const;
    std::string generate_help_filtered(const std::string& program_name, bool use_colors, const std::string& group_filter) const;
    std::string generate_help_filters(const std::string& program_name, bool use_colors) const;
    void collect_group_names(std::vector<std::string>& names, const std::string& prefix = "") const;
    void wrap_text(std::ostringstream& out, const std::string& text, size_t indent_col, size_t max_width) const;
    template<typename T> void append_usage_field(std::ostringstream& usage, const Config<T>& field) const;
    template<typename T> void append_usage_field(std::ostringstream& usage, const ConfigGroup<T>& group) const;
    template<typename T> void collect_field_info(const Config<T>& field, std::vector<FieldInfo>& required, std::vector<FieldInfo>& optional) const;
    template<typename T> void collect_field_info(const ConfigGroup<T>& group, std::vector<FieldInfo>& required, std::vector<FieldInfo>& optional) const;
    template<typename T> void collect_field_info_with_prefix(const Config<T>& field, const std::string& prefix, std::vector<FieldInfo>& required, std::vector<FieldInfo>& optional) const;
    template<typename T> void collect_field_info_with_prefix(const ConfigGroup<T>& group, const std::string& parent_prefix, std::vector<FieldInfo>& required, std::vector<FieldInfo>& optional) const;
    template<typename T> void print_field_hierarchical(std::ostringstream& out, const Config<T>& field, size_t indent, bool use_colors, size_t max_width, const std::string& prefix = "") const;
    template<typename T> void print_field_hierarchical(std::ostringstream& out, const ConfigGroup<T>& group, size_t indent, bool use_colors, size_t max_width, const std::string& prefix = "") const;
    template<typename T> void print_field_if_required(std::ostringstream& out, const Config<T>& field, bool use_colors, const std::string& prefix = "") const;
    template<typename T> void print_field_if_required(std::ostringstream& out, const ConfigGroup<T>& group, bool use_colors, const std::string& prefix = "") const;
    template<typename T> void print_group_structure(std::ostringstream& out, const Config<T>& field, size_t indent, bool use_colors, const std::string& prefix = "") const;
    template<typename T> void print_group_structure(std::ostringstream& out, const ConfigGroup<T>& group, size_t indent, bool use_colors, const std::string& prefix = "") const;
    template<typename T> bool print_field_if_matches(std::ostringstream& out, const Config<T>& field, bool use_colors, const std::string& filter, const std::string& prefix = "") const;
    template<typename T> bool print_field_if_matches(std::ostringstream& out, const ConfigGroup<T>& group, bool use_colors, const std::string& filter, const std::string& prefix = "") const;
    template<typename T> void collect_group_names_from_field(const Config<T>& field, std::vector<std::string>& names, const std::string& prefix) const;
    template<typename T> void collect_group_names_from_field(const ConfigGroup<T>& group, std::vector<std::string>& names, const std::string& prefix) const;
    
    template<typename T> bool try_set_field_value(Config<T>& field, const std::string& flag, const std::string& value);
    template<typename T> bool try_set_field_value(ConfigGroup<T>& group, const std::string& flag, const std::string& value);
    template<typename T> void validate_field(const Config<T>& field);
    template<typename T> void validate_field(const ConfigGroup<T>& group);
    template<typename T> void check_not_preset_flag(const Config<T>& field);
    template<typename T> void check_not_preset_flag(const ConfigGroup<T>& group);
    template<typename T> void dump_field(std::ostringstream& out, const Config<T>& field, bool only_changed) const;
    template<typename T> void dump_field(std::ostringstream& out, const ConfigGroup<T>& group, bool only_changed) const;
    template<typename T> void dump_field_toml(std::ostringstream& out, const Config<T>& field, bool only_changed) const;
    template<typename T> void dump_field_toml(std::ostringstream& out, const ConfigGroup<T>& group, bool only_changed) const;
    template<typename T> void load_field_from_parser(Config<T>& field, const PresetParser& parser);
    template<typename T> void load_field_from_parser(ConfigGroup<T>& group, const PresetParser& parser);
};

// Include implementation
#include "configs_loader_impl.hpp"
