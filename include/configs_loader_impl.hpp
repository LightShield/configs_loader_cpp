#pragma once

// Implementation details for ConfigsLoader
// This file is automatically included by configs_loader.hpp
// Users should not include this file directly

#include "parsers/preset_parser.hpp"
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <type_traits>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstdlib>
#include <memory>

// ANSI color codes for help output
namespace ansi {
    constexpr const char* RESET = "\033[0m";
    constexpr const char* BOLD = "\033[1m";
    constexpr const char* RED = "\033[31m";
    constexpr const char* GREEN = "\033[32m";
    constexpr const char* YELLOW = "\033[33m";
    constexpr const char* BLUE = "\033[34m";
    constexpr const char* MAGENTA = "\033[35m";
    constexpr const char* CYAN = "\033[36m";
    constexpr const char* GRAY = "\033[90m";
}

// Helper to conditionally apply colors
inline std::string colorize(const std::string& text, const char* color, bool use_colors) {
    if (!use_colors) return text;
    return std::string(color) + text + ansi::RESET;
}

// Helper to get type name as string
template<typename T>
inline std::string get_type_name() {
    if constexpr (std::is_same_v<T, std::string>) return "string";
    else if constexpr (std::is_same_v<T, int>) return "int";
    else if constexpr (std::is_same_v<T, bool>) return "bool";
    else if constexpr (std::is_same_v<T, double>) return "double";
    else return "unknown";
}

template<typename ConfigsType>
struct ConfigsLoader<ConfigsType>::FieldInfo {
    std::string prefix;
    std::string flags;
    std::string type;
    std::string description;
    std::string default_val;
};

template<typename ConfigsType>
ConfigsLoader<ConfigsType>::ConfigsLoader(int argc, char* argv[]) {
    init(argc, argv);
}

template<typename ConfigsType>
void ConfigsLoader<ConfigsType>::init(int argc, char* argv[]) {
    validate_no_preset_override();
    
    // Check for --help or -h flag
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            std::cout << generate_help(argv[0]) << std::endl;
            std::exit(0);
        }
    }
    
    if (argc > 1) {
        std::optional<std::string> preset_path = extract_preset_path(argc, argv);
        
        if (preset_path.has_value()) {
            load_preset_file(preset_path.value());
        }

        parse_cli_arguments(argc, argv);
    }
    
    validate_required_fields();
    m_initialized = true;
}

template<typename ConfigsType>
bool ConfigsLoader<ConfigsType>::is_initialized() const {
    return m_initialized;
}

template<typename ConfigsType>
std::string ConfigsLoader<ConfigsType>::dump_configs(bool only_changes) const {
    std::ostringstream dump;
    auto fields = configs.get_fields();
    std::apply([&](auto&... field) {
        ((dump_field(dump, field, only_changes)), ...);
    }, fields);
    return dump.str();
}

template<typename ConfigsType>
std::string ConfigsLoader<ConfigsType>::dump_to_toml(bool only_changes) const {
    std::ostringstream dump;
    auto fields = configs.get_fields();
    std::apply([&](auto&... field) {
        ((dump_field_toml(dump, field, only_changes)), ...);
    }, fields);
    return dump.str();
}

template<typename ConfigsType>
std::string ConfigsLoader<ConfigsType>::generate_help(const std::string& program_name, size_t max_width) const {
    std::ostringstream help;
    const bool use_colors = help_config.use_colors;
    
    // Build usage line
    help << colorize("Usage: ", ansi::BOLD, use_colors) << program_name << " [OPTIONS]";
    auto fields = configs.get_fields();
    std::apply([&](auto&... field) {
        ((append_usage_field(help, field)), ...);
    }, fields);
    help << "\n\n" << colorize("Options:", ansi::BOLD, use_colors) << "\n";
    
    // Built-in options
    help << "  " << colorize("--help, -h", ansi::CYAN, use_colors) 
         << "   " << colorize("<void>", ansi::YELLOW, use_colors) 
         << "  Show this help message\n";
    help << "  " << colorize("--preset, -p", ansi::CYAN, use_colors) 
         << " " << colorize("<file>", ansi::YELLOW, use_colors) 
         << "  Load configuration from JSON file (reserved)\n";
    
    // Print fields hierarchically
    std::apply([&](auto&... field) {
        ((print_field_hierarchical(help, field, 0, use_colors, max_width)), ...);
    }, fields);
    
    return help.str();
}

// Private method implementations

template<typename ConfigsType>
std::optional<std::string> ConfigsLoader<ConfigsType>::extract_preset_path(int argc, char* argv[]) {
    for (int i = 1; i < argc - 1; ++i) {
        std::string arg = argv[i];
        if (arg == "--preset" || arg == "-p") {
            return std::string(argv[i + 1]);
        }
        
        size_t equals_pos = arg.find('=');
        if (equals_pos != std::string::npos) {
            std::string flag = arg.substr(0, equals_pos);
            if (flag == "--preset" || flag == "-p") {
                return arg.substr(equals_pos + 1);
            }
        }
    }
    return std::nullopt;
}

template<typename ConfigsType>
void ConfigsLoader<ConfigsType>::load_preset_file(const std::string& path) {
    auto parser = create_preset_parser(path);
    parser->parse_file(path);
    
    auto fields = configs.get_fields();
    std::apply([&](auto&... field) {
        ((load_field_from_parser(field, *parser)), ...);
    }, fields);
}

template<typename ConfigsType>
void ConfigsLoader<ConfigsType>::parse_cli_arguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.empty() || arg[0] != '-') {
            continue;
        }

        // Skip preset flag - it's internal
        if (arg == "--preset" || arg == "-p") {
            ++i; // Skip the value too
            continue;
        }

        std::string value;
        bool has_value = false;

        size_t equals_pos = arg.find('=');
        if (equals_pos != std::string::npos) {
            std::string flag = arg.substr(0, equals_pos);
            if (flag == "--preset" || flag == "-p") {
                continue; // Skip preset
            }
            value = arg.substr(equals_pos + 1);
            arg = flag;
            has_value = true;
        } else if (i + 1 < argc && argv[i + 1][0] != '-') {
            value = argv[i + 1];
            has_value = true;
            ++i;
        }

        if (has_value) {
            try_set_config_value(arg, value);
        }
    }
}

template<typename ConfigsType>
template<typename T>
bool ConfigsLoader<ConfigsType>::try_set_field_value(Config<T>& field, const std::string& flag, const std::string& value) {
    const auto& flags = field.flags;
    if (std::find(flags.begin(), flags.end(), flag) == flags.end()) {
        return false;
    }

    T converted_value;
    if constexpr (std::is_same_v<T, std::string>) {
        converted_value = value;
    } else if constexpr (std::is_same_v<T, int>) {
        converted_value = std::stoi(value);
    } else if constexpr (std::is_same_v<T, bool>) {
        converted_value = (value == "true" || value == "1");
    } else if constexpr (std::is_same_v<T, double>) {
        converted_value = std::stod(value);
    } else {
        return false;
    }

    return field.set_value(converted_value);
}

template<typename ConfigsType>
void ConfigsLoader<ConfigsType>::try_set_config_value(const std::string& flag, const std::string& value) {
    auto fields = configs.get_fields();
    std::apply([&](auto&... field) {
        (try_set_field_value(field, flag, value) || ...);
    }, fields);
}

template<typename ConfigsType>
void ConfigsLoader<ConfigsType>::validate_required_fields() {
    auto fields = configs.get_fields();
    std::apply([&](auto&... field) {
        ((validate_field(field)), ...);
    }, fields);
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::validate_field(const Config<T>& field) {
    if (field.is_required() && !field.is_set()) {
        throw std::runtime_error("Required config field not set");
    }
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::validate_field(const ConfigGroup<T>& group) {
    auto fields = group.config.get_fields();
    std::apply([&](auto&... field) {
        ((validate_field(field)), ...);
    }, fields);
}

template<typename ConfigsType>
void ConfigsLoader<ConfigsType>::validate_no_preset_override() {
    auto fields = configs.get_fields();
    std::apply([&](auto&... field) {
        ((check_not_preset_flag(field)), ...);
    }, fields);
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::check_not_preset_flag(const Config<T>& field) {
    for (const auto& flag : field.flags) {
        if (flag == "--preset" || flag == "-p") {
            throw std::runtime_error("Config fields cannot use reserved --preset or -p flags");
        }
    }
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::check_not_preset_flag(const ConfigGroup<T>& group) {
    auto fields = group.config.get_fields();
    std::apply([&](auto&... field) {
        ((check_not_preset_flag(field)), ...);
    }, fields);
}

template<typename ConfigsType>
template<typename T>
bool ConfigsLoader<ConfigsType>::try_set_field_value(ConfigGroup<T>& group, const std::string& flag, const std::string& value) {
    // Extract flag prefix (-- or -)
    std::string flag_prefix;
    std::string flag_body;
    if (flag.starts_with("--")) {
        flag_prefix = "--";
        flag_body = flag.substr(2);
    } else if (flag.starts_with("-")) {
        flag_prefix = "-";
        flag_body = flag.substr(1);
    } else {
        flag_body = flag;
    }
    
    // Check if flag body starts with this group's prefix
    std::string prefix_with_dot = group.name_ + ".";
    if (flag_body.find(prefix_with_dot) != 0) {
        return false;
    }
    
    // Remove prefix and reconstruct flag with prefix
    std::string nested_flag_body = flag_body.substr(prefix_with_dot.length());
    std::string nested_flag = flag_prefix + nested_flag_body;
    
    auto fields = group.config.get_fields();
    return std::apply([&](auto&... field) {
        return (try_set_field_value(field, nested_flag, value) || ...);
    }, fields);
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::append_usage_field(std::ostringstream& usage, const Config<T>& field) const {
    if (field.flags.empty()) return;
    
    // Find shortest flag
    std::string shortest = field.flags[0];
    for (const auto& flag : field.flags) {
        if (flag.length() < shortest.length()) {
            shortest = flag;
        }
    }
    
    if (field.is_required()) {
        usage << " " << shortest << " <value>";
    } else {
        usage << " [" << shortest << " <value>]";
    }
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::collect_field_info(const Config<T>& field, 
                       std::vector<FieldInfo>& required_fields,
                       std::vector<FieldInfo>& optional_fields) const {
    if (field.flags.empty()) return;
    
    FieldInfo info;
    info.prefix = field.is_required() ? "[Required]" : "";
    
    std::ostringstream flags_str;
    for (size_t i = 0; i < field.flags.size(); ++i) {
        flags_str << field.flags[i];
        if (i < field.flags.size() - 1) flags_str << ", ";
    }
    info.flags = flags_str.str();
    
    info.type = "<";
    if constexpr (std::is_same_v<T, std::string>) {
        info.type += "string";
    } else if constexpr (std::is_same_v<T, int>) {
        info.type += "int";
    } else if constexpr (std::is_same_v<T, bool>) {
        info.type += "bool";
    } else if constexpr (std::is_same_v<T, double>) {
        info.type += "double";
    } else {
        info.type += "value";
    }
    info.type += ">";
    
    info.description = field.description.empty() ? "No description provided for this config" : field.description;
    
    std::ostringstream default_str;
    if constexpr (std::is_same_v<T, std::string>) {
        default_str << "\"" << field.default_value << "\"";
    } else if constexpr (std::is_same_v<T, bool>) {
        default_str << (field.default_value ? "true" : "false");
    } else {
        default_str << field.default_value;
    }
    info.default_val = default_str.str();
    
    if (field.is_required()) {
        required_fields.push_back(info);
    } else {
        optional_fields.push_back(info);
    }
}

// ConfigGroup overloads for help generation
template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::append_usage_field(std::ostringstream& usage, const ConfigGroup<T>& group) const {
    // Recursively append usage for nested fields - prefix will be handled in CLI parsing
    auto fields = group.config.get_fields();
    std::apply([&](auto&... field) {
        ((append_usage_field(usage, field)), ...);
    }, fields);
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::collect_field_info(const ConfigGroup<T>& group,
                       std::vector<FieldInfo>& required_fields,
                       std::vector<FieldInfo>& optional_fields) const {
    // Collect field info with prefix prepended to flags
    auto fields = group.config.get_fields();
    std::apply([&](auto&... field) {
        ((collect_field_info_with_prefix(field, group.name_, required_fields, optional_fields)), ...);
    }, fields);
}

// Helper to collect field info with prefix
template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::collect_field_info_with_prefix(const Config<T>& field,
                       const std::string& prefix,
                       std::vector<FieldInfo>& required_fields,
                       std::vector<FieldInfo>& optional_fields) const {
    if (field.flags.empty()) return;
    
    FieldInfo info;
    info.prefix = field.is_required() ? "[Required]" : "";
    
    // Prepend prefix to all flags
    std::ostringstream flags_str;
    for (size_t i = 0; i < field.flags.size(); ++i) {
        std::string flag = field.flags[i];
        // Insert prefix after -- or -
        if (flag.starts_with("--")) {
            flags_str << "--" << prefix << "." << flag.substr(2);
        } else if (flag.starts_with("-")) {
            flags_str << "-" << prefix << "." << flag.substr(1);
        } else {
            flags_str << prefix << "." << flag;
        }
        if (i < field.flags.size() - 1) flags_str << ", ";
    }
    info.flags = flags_str.str();
    
    info.type = "<";
    if constexpr (std::is_same_v<T, std::string>) {
        info.type += "string";
    } else if constexpr (std::is_same_v<T, int>) {
        info.type += "int";
    } else if constexpr (std::is_same_v<T, bool>) {
        info.type += "bool";
    } else if constexpr (std::is_same_v<T, double>) {
        info.type += "double";
    } else {
        info.type += "value";
    }
    info.type += ">";
    
    info.description = field.description.empty() ? "No description provided for this config" : field.description;
    
    std::ostringstream default_str;
    if constexpr (std::is_same_v<T, std::string>) {
        default_str << "\"" << field.default_value << "\"";
    } else if constexpr (std::is_same_v<T, bool>) {
        default_str << (field.default_value ? "true" : "false");
    } else {
        default_str << field.default_value;
    }
    info.default_val = default_str.str();
    
    if (field.is_required()) {
        required_fields.push_back(info);
    } else {
        optional_fields.push_back(info);
    }
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::collect_field_info_with_prefix(const ConfigGroup<T>& group,
                       const std::string& parent_prefix,
                       std::vector<FieldInfo>& required_fields,
                       std::vector<FieldInfo>& optional_fields) const {
    // Accumulate prefix
    std::string full_prefix = parent_prefix.empty() ? group.name_ : parent_prefix + "." + group.name_;
    auto fields = group.config.get_fields();
    std::apply([&](auto&... field) {
        ((collect_field_info_with_prefix(field, full_prefix, required_fields, optional_fields)), ...);
    }, fields);
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::dump_field(std::ostringstream& out, const Config<T>& field, bool only_changed) const {
    if (field.flags.empty()) return;
    
    // Skip if only dumping changes and value equals default
    if (only_changed && field.value == field.default_value) {
        return;
    }
    
    // Use first flag as the key
    out << field.flags[0] << "=";
    
    if constexpr (std::is_same_v<T, std::string>) {
        out << "\"" << field.value << "\"";
    } else if constexpr (std::is_same_v<T, bool>) {
        out << (field.value ? "true" : "false");
    } else {
        out << field.value;
    }
    
    out << "\n";
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::dump_field_toml(std::ostringstream& out, const Config<T>& field, bool only_changed) const {
    if (field.flags.empty()) return;
    
    // Skip if only dumping changes and value equals default
    if (only_changed && field.value == field.default_value) {
        return;
    }
    
    // Use first flag without dashes as TOML key
    std::string key = field.flags[0];
    if (key.starts_with("--")) {
        key = key.substr(2);
    } else if (key.starts_with("-")) {
        key = key.substr(1);
    }
    
    out << key << " = ";
    
    if constexpr (std::is_same_v<T, std::string>) {
        out << "\"" << field.value << "\"";
    } else if constexpr (std::is_same_v<T, bool>) {
        out << (field.value ? "true" : "false");
    } else {
        out << field.value;
    }
    
    out << "\n";
}

template<typename ConfigsType>
void ConfigsLoader<ConfigsType>::wrap_text(std::ostringstream& out, const std::string& text, size_t indent_col, size_t max_width) const {
    if (text.empty()) return;
    
    size_t available = max_width - indent_col;
    if (text.length() <= available) {
        out << text;
        return;
    }
    
    size_t pos = 0;
    bool first_line = true;
    
    while (pos < text.length()) {
        if (!first_line) {
            out << "\n" << std::string(indent_col, ' ');
        }
        first_line = false;
        
        size_t remaining = text.length() - pos;
        if (remaining <= available) {
            out << text.substr(pos);
            break;
        }
        
        size_t break_pos = text.rfind(' ', pos + available);
        if (break_pos == std::string::npos || break_pos <= pos) {
            break_pos = pos + available;
        }
        
        out << text.substr(pos, break_pos - pos);
        pos = break_pos;
        while (pos < text.length() && text[pos] == ' ') ++pos;
    }
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::load_field_from_parser(Config<T>& field, const PresetParser& parser) {
    if (field.flags.empty()) return;
    
    // Try all flags as keys (without dashes)
    for (const auto& flag : field.flags) {
        std::string key = flag;
        if (key.starts_with("--")) {
            key = key.substr(2);
        } else if (key.starts_with("-")) {
            key = key.substr(1);
        }
        
        std::optional<T> value;
        if constexpr (std::is_same_v<T, std::string>) {
            value = parser.get_string(key);
        } else if constexpr (std::is_same_v<T, int>) {
            value = parser.get_int(key);
        } else if constexpr (std::is_same_v<T, bool>) {
            value = parser.get_bool(key);
        } else if constexpr (std::is_same_v<T, double>) {
            value = parser.get_double(key);
        }
        
        if (value.has_value()) {
            field.set_value(*value);
            return; // Found value, stop trying other flags
        }
    }
}

// ConfigGroup overloads - recursively process nested configs with prefix
template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::dump_field(std::ostringstream& out, const ConfigGroup<T>& group, bool only_changed) const {
    auto fields = group.config.get_fields();
    std::apply([&](auto&... field) {
        ((dump_field(out, field, only_changed)), ...);
    }, fields);
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::dump_field_toml(std::ostringstream& out, const ConfigGroup<T>& group, bool only_changed) const {
    auto fields = group.config.get_fields();
    std::apply([&](auto&... field) {
        ((dump_field_toml(out, field, only_changed)), ...);
    }, fields);
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::load_field_from_parser(ConfigGroup<T>& group, const PresetParser& parser) {
    auto fields = group.config.get_fields();
    std::apply([&](auto&... field) {
        ((load_field_from_parser(field, parser)), ...);
    }, fields);
}

// Hierarchical help printing
template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::print_field_hierarchical(std::ostringstream& out, const Config<T>& field, size_t indent, bool use_colors, size_t, const std::string& prefix) const {
    if (field.flags.empty()) return;
    
    std::string indent_str(indent * 2, ' ');
    
    // Format: "  --flag, -f  <type>  description (default: value)"
    out << "  " << indent_str;
    
    // Required marker
    if (field.is_required()) {
        out << colorize("[Required] ", ansi::RED, use_colors);
    }
    
    // Flags with prefix
    std::ostringstream flags_str;
    for (size_t i = 0; i < field.flags.size(); ++i) {
        if (i > 0) flags_str << ", ";
        std::string flag = field.flags[i];
        if (!prefix.empty()) {
            // Insert prefix after -- or -
            if (flag.starts_with("--")) {
                flags_str << "--" << prefix << "." << flag.substr(2);
            } else if (flag.starts_with("-") && flag.length() == 2) {
                flags_str << flag;  // Short flags don't get prefix
            } else {
                flags_str << flag;
            }
        } else {
            flags_str << flag;
        }
    }
    out << colorize(flags_str.str(), ansi::CYAN, use_colors);
    
    // Type
    out << "  " << colorize("<" + get_type_name<T>() + ">", ansi::YELLOW, use_colors);
    
    // Description
    std::string desc = field.description.empty() ? "No description provided for this config" : field.description;
    out << "  " << desc;
    
    // Default value
    if constexpr (std::is_same_v<T, std::string>) {
        out << " " << colorize("(default: \"" + field.default_value + "\")", ansi::GRAY, use_colors);
    } else if constexpr (std::is_same_v<T, bool>) {
        out << " " << colorize("(default: " + std::string(field.default_value ? "true" : "false") + ")", ansi::GRAY, use_colors);
    } else {
        out << " " << colorize("(default: " + std::to_string(field.default_value) + ")", ansi::GRAY, use_colors);
    }
    
    out << "\n";
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::print_field_hierarchical(std::ostringstream& out, const ConfigGroup<T>& group, size_t indent, bool use_colors, size_t max_width, const std::string& prefix) const {
    std::string indent_str(indent * 2, ' ');
    
    // Group header
    out << "  " << indent_str << colorize(group.name_ + ":", ansi::GREEN, use_colors) << "\n";
    
    // Build full prefix for nested fields
    std::string full_prefix = prefix.empty() ? group.name_ : prefix + "." + group.name_;
    
    // Print nested fields with increased indentation
    auto fields = group.config.get_fields();
    std::apply([&](auto&... field) {
        ((print_field_hierarchical(out, field, indent + 1, use_colors, max_width, full_prefix)), ...);
    }, fields);
}
