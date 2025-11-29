#pragma once

#include "help_colors.hpp"
#include <sstream>
#include <vector>
#include <algorithm>

template<typename ConfigsType>
std::string ConfigsLoader<ConfigsType>::generate_help(const std::string& program_name, size_t max_width, const std::string& filter) const {
    std::ostringstream help;
    const bool use_colors = help_config.use_colors;
    
    if (!filter.empty()) {
        if (filter == "required") {
            return generate_help_required(program_name, use_colors);
        } else if (filter == "filters" || filter == "help") {
            return generate_help_filters(program_name, use_colors);
        } else if (filter == "groups") {
            return generate_help_groups(program_name, use_colors);
        } else if (filter == "all") {
            // Fall through to show full help
        } else {
            return generate_help_filtered(program_name, use_colors, filter);
        }
    } else if (help_config.enable_interactive) {
        return generate_help_navigation(program_name, use_colors);
    }
    
    help << colorize("Usage: ", ansi::BOLD, use_colors) << program_name << " [OPTIONS]";
    auto fields = configs.get_fields();
    std::apply([&](auto&... field) {
        ((append_usage_field(help, field)), ...);
    }, fields);
    help << "\n\n" << colorize("Options:", ansi::BOLD, use_colors) << "\n";
    
    std::string help_desc = "Show this help message";
    if (help_config.enable_interactive) {
        help_desc += " (use --help <filter> for filtered help)";
    }
    help << "  " << colorize("--help, -h", ansi::CYAN, use_colors) 
         << "   " << colorize("<void>", ansi::YELLOW, use_colors) 
         << "  " << help_desc << "\n";
    help << "  " << colorize("--preset, -p", ansi::CYAN, use_colors) 
         << " " << colorize("<file>", ansi::YELLOW, use_colors) 
         << "  Load configuration from JSON file (reserved)\n";
    
    std::apply([&](auto&... field) {
        ((print_field_hierarchical(help, field, 0, use_colors, max_width)), ...);
    }, fields);
    
    if (help_config.enable_interactive) {
        help << "\n" << colorize("Interactive Help:", ansi::BOLD, use_colors) << "\n";
        help << "  --help all           Show all configuration options\n";
        help << "  --help required      Show only required fields\n";
        help << "  --help <group>       Show only fields in specific group\n";
        help << "  --help filters       Show all available filters\n";
    }
    
    return help.str();
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::append_usage_field(std::ostringstream& usage, const Config<T>& field) const {
    if (field.is_required() && !field.flags.empty()) {
        usage << " " << field.flags[0] << " <" << get_type_name<T>() << ">";
    }
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::collect_field_info(const Config<T>& field, 
                                                     std::vector<FieldInfo>& required, 
                                                     std::vector<FieldInfo>& optional) const {
    collect_field_info_with_prefix(field, "", required, optional);
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::append_usage_field(std::ostringstream& usage, const ConfigGroup<T>& group) const {
    auto fields = group.config.get_fields();
    std::apply([&](auto&... field) {
        ((append_usage_field(usage, field)), ...);
    }, fields);
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::collect_field_info(const ConfigGroup<T>& group,
                                                     std::vector<FieldInfo>& required,
                                                     std::vector<FieldInfo>& optional) const {
    collect_field_info_with_prefix(group, "", required, optional);
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::collect_field_info_with_prefix(const Config<T>& field,
                                                                 const std::string& prefix,
                                                                 std::vector<FieldInfo>& required,
                                                                 std::vector<FieldInfo>& optional) const {
    if (field.flags.empty()) return;
    
    FieldInfo info;
    info.prefix = prefix;
    
    std::ostringstream flags_str;
    for (size_t i = 0; i < field.flags.size(); ++i) {
        if (i > 0) flags_str << ", ";
        std::string flag = field.flags[i];
        if (!prefix.empty() && flag.starts_with("--")) {
            flag = "--" + prefix + "." + flag.substr(2);
        }
        flags_str << flag;
    }
    info.flags = flags_str.str();
    info.type = get_type_name<T>();
    info.description = field.description;
    
    if constexpr (std::is_same_v<T, std::string>) {
        info.default_val = "\"" + field.default_value + "\"";
    } else if constexpr (std::is_same_v<T, bool>) {
        info.default_val = field.default_value ? "true" : "false";
    } else {
        info.default_val = std::to_string(field.default_value);
    }
    
    if (field.is_required()) {
        required.push_back(info);
    } else {
        optional.push_back(info);
    }
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::collect_field_info_with_prefix(const ConfigGroup<T>& group,
                                                                 const std::string& parent_prefix,
                                                                 std::vector<FieldInfo>& required,
                                                                 std::vector<FieldInfo>& optional) const {
    std::string full_prefix = parent_prefix.empty() ? group.name_ : parent_prefix + "." + group.name_;
    auto fields = group.config.get_fields();
    std::apply([&](auto&... field) {
        ((collect_field_info_with_prefix(field, full_prefix, required, optional)), ...);
    }, fields);
}

template<typename ConfigsType>
void ConfigsLoader<ConfigsType>::wrap_text(std::ostringstream& out, const std::string& text, size_t indent_col, size_t max_width) const {
    if (text.empty()) return;
    
    size_t available_width = max_width > indent_col ? max_width - indent_col : 40;
    std::string indent(indent_col, ' ');
    
    size_t pos = 0;
    bool first_line = true;
    
    while (pos < text.length()) {
        if (!first_line) {
            out << "\n" << indent;
        }
        first_line = false;
        
        size_t remaining = text.length() - pos;
        if (remaining <= available_width) {
            out << text.substr(pos);
            break;
        }
        
        size_t break_pos = text.rfind(' ', pos + available_width);
        if (break_pos == std::string::npos || break_pos <= pos) {
            break_pos = pos + available_width;
        }
        
        out << text.substr(pos, break_pos - pos);
        pos = break_pos;
        while (pos < text.length() && text[pos] == ' ') {
            ++pos;
        }
    }
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::print_field_hierarchical(std::ostringstream& out, const Config<T>& field, size_t indent, bool use_colors, size_t, const std::string& prefix) const {
    if (field.flags.empty()) return;
    
    std::string indent_str(indent * 2, ' ');
    out << "  " << indent_str;
    
    if (field.is_required()) {
        out << colorize("[Required] ", ansi::RED, use_colors);
    }
    
    std::ostringstream flags_str;
    for (size_t i = 0; i < field.flags.size(); ++i) {
        if (i > 0) flags_str << ", ";
        std::string flag = field.flags[i];
        if (!prefix.empty() && flag.starts_with("--")) {
            flag = "--" + prefix + "." + flag.substr(2);
        }
        flags_str << flag;
    }
    out << colorize(flags_str.str(), ansi::CYAN, use_colors);
    
    out << "  " << colorize("<" + get_type_name<T>() + ">", ansi::YELLOW, use_colors);
    
    std::string desc = field.description.empty() ? "No description provided for this config" : field.description;
    out << "  " << desc;
    
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
    
    out << "  " << indent_str << colorize(group.name_ + ":", ansi::GREEN, use_colors) << "\n";
    
    std::string full_prefix = prefix.empty() ? group.name_ : prefix + "." + group.name_;
    
    auto fields = group.config.get_fields();
    std::apply([&](auto&... field) {
        ((print_field_hierarchical(out, field, indent + 1, use_colors, max_width, full_prefix)), ...);
    }, fields);
}

template<typename ConfigsType>
std::string ConfigsLoader<ConfigsType>::generate_help_navigation(const std::string& program_name, bool use_colors) const {
    std::ostringstream help;
    
    help << colorize(program_name, ansi::BOLD, use_colors) << " has many configuration options.\n\n";
    help << "Interactive help is enabled to help you navigate its usage.\n";
    help << "Run " << colorize("--help", ansi::CYAN, use_colors) << " with one of the following:\n\n";
    
    help << "  " << colorize("all", ansi::CYAN, use_colors) << "       - Show all configuration options\n";
    help << "  " << colorize("groups", ansi::CYAN, use_colors) << "    - Show only the configuration group structure\n";
    help << "  " << colorize("required", ansi::CYAN, use_colors) << "  - Show only required fields\n";
    help << "  " << colorize("filters", ansi::CYAN, use_colors) << "   - List all available groups and filters\n";
    help << "  " << colorize("<group>", ansi::CYAN, use_colors) << "   - Show only a specific configuration group\n";
    
    help << "\n" << colorize("Examples:", ansi::BOLD, use_colors) << "\n";
    help << "  " << program_name << " --help " << colorize("all", ansi::CYAN, use_colors) << "\n";
    help << "  " << program_name << " --help " << colorize("groups", ansi::CYAN, use_colors) << "\n";
    help << "  " << program_name << " --help " << colorize("required", ansi::CYAN, use_colors) << "\n";
    help << "  " << program_name << " --help " << colorize("filters", ansi::CYAN, use_colors) << "\n";
    
    return help.str();
}

template<typename ConfigsType>
std::string ConfigsLoader<ConfigsType>::generate_help_required(const std::string& program_name, bool use_colors) const {
    std::ostringstream help;
    help << colorize("Required Fields for ", ansi::BOLD, use_colors) << program_name << ":\n\n";
    
    std::ostringstream fields_out;
    auto fields = configs.get_fields();
    std::apply([&](auto&... field) {
        ((print_field_if_required(fields_out, field, use_colors)), ...);
    }, fields);
    
    if (fields_out.str().empty()) {
        help << "No required fields found.\n";
    } else {
        help << fields_out.str();
    }
    
    return help.str();
}

template<typename ConfigsType>
std::string ConfigsLoader<ConfigsType>::generate_help_groups(const std::string& program_name, bool use_colors) const {
    std::ostringstream help;
    help << colorize("Configuration Groups for ", ansi::BOLD, use_colors) << program_name << ":\n\n";
    
    auto fields = configs.get_fields();
    std::apply([&](auto&... field) {
        ((print_group_structure(help, field, 0, use_colors)), ...);
    }, fields);
    
    return help.str();
}

template<typename ConfigsType>
std::string ConfigsLoader<ConfigsType>::generate_help_filtered(const std::string&, bool use_colors, const std::string& group_filter) const {
    std::ostringstream help;
    help << colorize("Help for group '", ansi::BOLD, use_colors) << group_filter << "':\n\n";
    
    bool found = false;
    auto fields = configs.get_fields();
    std::apply([&](auto&... field) {
        ((found = print_field_if_matches(help, field, use_colors, group_filter) || found), ...);
    }, fields);
    
    if (!found) {
        help << "No group found matching '" << group_filter << "'\n";
        help << "Use --help filters to see available groups\n";
    }
    
    return help.str();
}

template<typename ConfigsType>
std::string ConfigsLoader<ConfigsType>::generate_help_filters(const std::string&, bool use_colors) const {
    std::ostringstream help;
    help << colorize("Available Help Filters:", ansi::BOLD, use_colors) << "\n\n";
    help << colorize("  all", ansi::CYAN, use_colors) << "       - Show all configuration options\n";
    help << colorize("  groups", ansi::CYAN, use_colors) << "    - Show only the configuration group structure\n";
    help << colorize("  required", ansi::CYAN, use_colors) << "  - Show only required fields\n";
    
    std::vector<std::string> group_names;
    collect_group_names(group_names);
    
    if (!group_names.empty()) {
        help << "\n" << colorize("Available Groups:", ansi::BOLD, use_colors) << "\n";
        for (const auto& name : group_names) {
            help << colorize("  " + name, ansi::CYAN, use_colors) << "\n";
        }
    }
    
    return help.str();
}

template<typename ConfigsType>
void ConfigsLoader<ConfigsType>::collect_group_names(std::vector<std::string>& names, const std::string& prefix) const {
    auto fields = configs.get_fields();
    std::apply([&](auto&... field) {
        ((collect_group_names_from_field(field, names, prefix)), ...);
    }, fields);
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::print_field_if_required(std::ostringstream& out, const Config<T>& field, bool use_colors, const std::string& prefix) const {
    if (!field.is_required() || field.flags.empty()) return;
    
    std::string flag = field.flags[0];
    if (!prefix.empty() && flag.starts_with("--")) {
        flag = "--" + prefix + "." + flag.substr(2);
    }
    
    out << "  " << colorize(flag, ansi::CYAN, use_colors) 
        << "  " << colorize("<" + get_type_name<T>() + ">", ansi::YELLOW, use_colors)
        << "  " << field.description << "\n";
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::print_field_if_required(std::ostringstream& out, const ConfigGroup<T>& group, bool use_colors, const std::string& prefix) const {
    std::string full_prefix = prefix.empty() ? group.name_ : prefix + "." + group.name_;
    auto fields = group.config.get_fields();
    std::apply([&](auto&... field) {
        ((print_field_if_required(out, field, use_colors, full_prefix)), ...);
    }, fields);
}

template<typename ConfigsType>
template<typename T>
bool ConfigsLoader<ConfigsType>::print_field_if_matches(std::ostringstream&, const Config<T>&, bool, const std::string&, const std::string&) const {
    return false;
}

template<typename ConfigsType>
template<typename T>
bool ConfigsLoader<ConfigsType>::print_field_if_matches(std::ostringstream& out, const ConfigGroup<T>& group, bool use_colors, const std::string& filter, const std::string& prefix) const {
    std::string full_prefix = prefix.empty() ? group.name_ : prefix + "." + group.name_;
    
    if (group.name_ == filter || full_prefix == filter) {
        print_field_hierarchical(out, group, 0, use_colors, 80, prefix);
        return true;
    }
    
    bool found = false;
    auto fields = group.config.get_fields();
    std::apply([&](auto&... field) {
        ((found = print_field_if_matches(out, field, use_colors, filter, full_prefix) || found), ...);
    }, fields);
    
    return found;
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::collect_group_names_from_field(const Config<T>&, std::vector<std::string>&, const std::string&) const {
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::collect_group_names_from_field(const ConfigGroup<T>& group, std::vector<std::string>& names, const std::string& prefix) const {
    std::string full_name = prefix.empty() ? group.name_ : prefix + "." + group.name_;
    names.push_back(full_name);
    
    auto fields = group.config.get_fields();
    std::apply([&](auto&... field) {
        ((collect_group_names_from_field(field, names, full_name)), ...);
    }, fields);
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::print_group_structure(std::ostringstream&, const Config<T>&, size_t, bool, const std::string&) const {
}

template<typename ConfigsType>
template<typename T>
void ConfigsLoader<ConfigsType>::print_group_structure(std::ostringstream& out, const ConfigGroup<T>& group, size_t indent, bool use_colors, const std::string& prefix) const {
    std::string indent_str(indent * 2, ' ');
    std::string full_prefix = prefix.empty() ? group.name_ : prefix + "." + group.name_;
    
    out << "  " << indent_str << colorize(group.name_, ansi::GREEN, use_colors) 
        << " " << colorize("(" + full_prefix + ")", ansi::GRAY, use_colors) << "\n";
    
    auto fields = group.config.get_fields();
    std::apply([&](auto&... field) {
        ((print_group_structure(out, field, indent + 1, use_colors, full_prefix)), ...);
    }, fields);
}
