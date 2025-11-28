#pragma once

#include "config.hpp"
#include <string>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <type_traits>
#include <vector>
#include <tuple>

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

    ConfigsLoader() = default;

    ConfigsLoader(int argc, char* argv[]) {
        Init(argc, argv);
    }

    void Init(int argc, char* argv[]) {
        validate_no_preset_override();
        
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

    [[nodiscard]] bool is_initialized() const { return m_initialized; }

    // Generate help text from registered configs
    [[nodiscard]] std::string generate_help(const std::string& program_name = "program", size_t max_width = 80) const {
        std::ostringstream help;
        
        // Build usage line with short flags
        help << "Usage: " << program_name << " [OPTIONS]";
        auto fields = configs.get_fields();
        std::apply([&](auto&... field) {
            ((append_usage_field(help, field)), ...);
        }, fields);
        help << "\n\nOptions:\n";
        
        // Collect field info for alignment calculation
        std::vector<FieldInfo> all_fields;
        
        all_fields.push_back({"", "--help, -h", "<void>", "Show this help message", ""});
        all_fields.push_back({"", "--preset, -p", "<file>", "Load configuration from JSON file (reserved)", ""});
        
        std::vector<FieldInfo> required_fields;
        std::vector<FieldInfo> optional_fields;
        std::apply([&](auto&... field) {
            ((collect_field_info(field, required_fields, optional_fields)), ...);
        }, fields);
        
        all_fields.insert(all_fields.end(), required_fields.begin(), required_fields.end());
        all_fields.insert(all_fields.end(), optional_fields.begin(), optional_fields.end());
        
        // Calculate column widths
        size_t prefix_width = 0, flags_width = 0, type_width = 0;
        for (const auto& f : all_fields) {
            prefix_width = std::max(prefix_width, f.prefix.length());
            flags_width = std::max(flags_width, f.flags.length());
            type_width = std::max(type_width, f.type.length());
        }
        
        // Format and output
        const size_t desc_col = 2 + prefix_width + (prefix_width > 0 ? 1 : 0) + flags_width + 1 + type_width + 4;
        
        for (const auto& f : all_fields) {
            help << "  ";
            
            // Prefix column (e.g., [Required])
            if (prefix_width > 0) {
                help << f.prefix << std::string(prefix_width - f.prefix.length(), ' ') << " ";
            }
            
            // Flags column
            help << f.flags << std::string(flags_width - f.flags.length(), ' ') << " ";
            
            // Type column
            help << f.type << std::string(type_width - f.type.length(), ' ');
            
            help << "    ";
            
            std::string full_desc = f.description;
            if (!f.default_val.empty()) {
                full_desc += " (default: " + f.default_val + ")";
            }
            
            wrap_text(help, full_desc, desc_col, max_width);
            help << "\n";
        }
        
        return help.str();
    }

private:
    std::optional<std::string> extract_preset_path(int argc, char* argv[]) {
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

    void load_preset_file([[maybe_unused]] const std::string& path) {
        // TODO: Implement JSON parsing
    }

    void parse_cli_arguments(int argc, char* argv[]) {
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

    template<typename T>
    bool try_set_field_value(Config<T>& field, const std::string& flag, const std::string& value) {
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

    void try_set_config_value(const std::string& flag, const std::string& value) {
        auto fields = configs.get_fields();
        std::apply([&](auto&... field) {
            (try_set_field_value(field, flag, value) || ...);
        }, fields);
    }

    void validate_required_fields() {
        auto fields = configs.get_fields();
        std::apply([&](auto&... field) {
            ((validate_field(field)), ...);
        }, fields);
    }

    template<typename T>
    void validate_field(const Config<T>& field) {
        if (field.is_required() && !field.is_set()) {
            throw std::runtime_error("Required config field not set");
        }
    }

    void validate_no_preset_override() {
        auto fields = configs.get_fields();
        std::apply([&](auto&... field) {
            ((check_not_preset_flag(field)), ...);
        }, fields);
    }

    template<typename T>
    void check_not_preset_flag(const Config<T>& field) {
        for (const auto& flag : field.flags) {
            if (flag == "--preset" || flag == "-p") {
                throw std::runtime_error("Config fields cannot use reserved --preset or -p flags");
            }
        }
    }

    struct FieldInfo {
        std::string prefix;
        std::string flags;
        std::string type;
        std::string description;
        std::string default_val;
    };

    template<typename T>
    void append_usage_field(std::ostringstream& usage, const Config<T>& field) const {
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

    template<typename T>
    void collect_field_info(const Config<T>& field, 
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

    void wrap_text(std::ostringstream& out, const std::string& text, size_t indent_col, size_t max_width) const {
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
    
    bool m_initialized = false;
};
