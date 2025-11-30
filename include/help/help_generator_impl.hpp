#pragma once

template<typename ConfigsType>
std::string HelpGenerator<ConfigsType>::generate(const std::string& program_name, size_t max_width, const std::string& filter) const {
    if (!filter.empty()) {
        if (filter == "required") {
            return generate_required(program_name);
        } else if (filter == "filters" || filter == "help") {
            return generate_filters(program_name);
        } else if (filter == "groups") {
            return generate_groups(program_name);
        } else if (filter == "all") {
            // Fall through to show full help
        } else {
            return generate_filtered(program_name, filter);
        }
    } else if (m_enable_interactive) {
        return generate_navigation(program_name);
    }
    
    std::ostringstream help;
    help << colorize("Usage: ", ansi::BOLD, m_use_colors) << program_name << " [OPTIONS]";
    auto fields = m_configs.get_fields();
    std::apply([&](auto&... field) {
        ((append_usage_field(help, field)), ...);
    }, fields);
    help << "\n\n" << colorize("Options:", ansi::BOLD, m_use_colors) << "\n";
    
    const std::string help_desc = m_enable_interactive 
        ? "Show this help message (use --help <filter> for filtered help)"
        : "Show this help message";
    help << "  " << colorize("--help, -h", ansi::CYAN, m_use_colors) 
         << "   " << colorize("<void>", ansi::YELLOW, m_use_colors) 
         << "  " << help_desc << "\n";
    help << "  " << colorize("--preset, -p", ansi::CYAN, m_use_colors) 
         << " " << colorize("<file>", ansi::YELLOW, m_use_colors) 
         << "  Load configuration from JSON file (reserved)\n";
    
    std::apply([&](auto&... field) {
        ((print_field_hierarchical(help, field, 0, max_width)), ...);
    }, fields);
    
    if (m_enable_interactive) {
        help << "\n" << colorize("Interactive Help:", ansi::BOLD, m_use_colors) << "\n";
        help << "  --help all           Show all configuration options\n";
        help << "  --help required      Show only required fields\n";
        help << "  --help <group>       Show only fields in specific group\n";
        help << "  --help filters       Show all available filters\n";
    }
    
    return help.str();
}

template<typename ConfigsType>
template<typename T>
void HelpGenerator<ConfigsType>::append_usage_field(std::ostringstream& usage, const Config<T>& field) const {
    if (field.is_required() && !field.flags.empty()) {
        usage << " " << field.flags[0] << " <" << get_type_name<T>() << ">";
    }
}

template<typename ConfigsType>
template<typename T>
void HelpGenerator<ConfigsType>::append_usage_field(std::ostringstream& usage, const ConfigGroup<T>& group) const {
    auto fields = group.get_fields();
    std::apply([&](auto&... field) {
        ((append_usage_field(usage, field)), ...);
    }, fields);
}

template<typename ConfigsType>
template<typename T>
void HelpGenerator<ConfigsType>::print_field_hierarchical(std::ostringstream& out, const Config<T>& field, size_t indent, size_t, const std::string& prefix) const {
    if (field.flags.empty()) return;
    
    const std::string indent_str(indent * 2, ' ');
    out << "  " << indent_str;
    
    if (field.is_required()) {
        out << colorize("[Required] ", ansi::RED, m_use_colors);
    }
    
    std::ostringstream flags_str;
    for (size_t i = 0; i < field.flags.size(); ++i) {
        if (i > 0) flags_str << ", ";
        const std::string flag = [&]() {
            if (!prefix.empty() && field.flags[i].starts_with("--")) {
                return "--" + prefix + "." + field.flags[i].substr(2);
            }
            return field.flags[i];
        }();
        flags_str << flag;
    }
    out << colorize(flags_str.str(), ansi::CYAN, m_use_colors);
    
    out << "  " << colorize("<" + std::string(get_type_name<T>()) + ">", ansi::YELLOW, m_use_colors);
    
    const std::string desc = field.description.empty() ? "No description provided for this config" : field.description;
    out << "  " << desc;
    
    if constexpr (std::is_same_v<T, std::string>) {
        out << " " << colorize("(default: \"" + field.default_value + "\")", ansi::GRAY, m_use_colors);
    } else if constexpr (std::is_same_v<T, bool>) {
        out << " " << colorize("(default: " + std::string(field.default_value ? "true" : "false") + ")", ansi::GRAY, m_use_colors);
    } else {
        out << " " << colorize("(default: " + std::to_string(field.default_value) + ")", ansi::GRAY, m_use_colors);
    }
    
    out << "\n";
}

template<typename ConfigsType>
template<typename T>
void HelpGenerator<ConfigsType>::print_field_hierarchical(std::ostringstream& out, const ConfigGroup<T>& group, size_t indent, size_t max_width, const std::string& prefix) const {
    const std::string indent_str(indent * 2, ' ');
    
    out << "  " << indent_str << colorize(group.get_name() + ":", ansi::GREEN, m_use_colors) << "\n";
    
    const std::string full_prefix = prefix.empty() ? group.get_name() : prefix + "." + group.get_name();
    
    auto fields = group.get_fields();
    std::apply([&](auto&... field) {
        ((print_field_hierarchical(out, field, indent + 1, max_width, full_prefix)), ...);
    }, fields);
}

template<typename ConfigsType>
std::string HelpGenerator<ConfigsType>::generate_navigation(const std::string& program_name) const {
    std::ostringstream help;
    
    help << colorize(program_name, ansi::BOLD, m_use_colors) << " has many configuration options.\n\n";
    help << "Interactive help is enabled to help you navigate its usage.\n";
    help << "Run " << colorize("--help", ansi::CYAN, m_use_colors) << " with one of the following:\n\n";
    
    help << "  " << colorize("all", ansi::CYAN, m_use_colors) << "       - Show all configuration options\n";
    help << "  " << colorize("groups", ansi::CYAN, m_use_colors) << "    - Show only the configuration group structure\n";
    help << "  " << colorize("required", ansi::CYAN, m_use_colors) << "  - Show only required fields\n";
    help << "  " << colorize("filters", ansi::CYAN, m_use_colors) << "   - List all available groups and filters\n";
    help << "  " << colorize("<group>", ansi::CYAN, m_use_colors) << "   - Show only a specific configuration group\n";
    
    help << "\n" << colorize("Examples:", ansi::BOLD, m_use_colors) << "\n";
    help << "  " << program_name << " --help " << colorize("all", ansi::CYAN, m_use_colors) << "\n";
    help << "  " << program_name << " --help " << colorize("groups", ansi::CYAN, m_use_colors) << "\n";
    help << "  " << program_name << " --help " << colorize("required", ansi::CYAN, m_use_colors) << "\n";
    help << "  " << program_name << " --help " << colorize("filters", ansi::CYAN, m_use_colors) << "\n";
    
    return help.str();
}

template<typename ConfigsType>
template<typename T>
void HelpGenerator<ConfigsType>::print_field_if_required(std::ostringstream& out, const Config<T>& field, const std::string& prefix) const {
    if (!field.is_required() || field.flags.empty()) return;
    
    const std::string flag = [&]() {
        if (!prefix.empty() && field.flags[0].starts_with("--")) {
            return "--" + prefix + "." + field.flags[0].substr(2);
        }
        return field.flags[0];
    }();
    
    out << "  " << colorize(flag, ansi::CYAN, m_use_colors) 
        << "  " << colorize(std::string("<") + get_type_name<T>() + ">", ansi::YELLOW, m_use_colors)
        << "  " << field.description << "\n";
}

template<typename ConfigsType>
template<typename T>
void HelpGenerator<ConfigsType>::print_field_if_required(std::ostringstream& out, const ConfigGroup<T>& group, const std::string& prefix) const {
    const std::string full_prefix = prefix.empty() ? group.get_name() : prefix + "." + group.get_name();
    auto fields = group.get_fields();
    std::apply([&](auto&... field) {
        ((print_field_if_required(out, field, full_prefix)), ...);
    }, fields);
}

template<typename ConfigsType>
std::string HelpGenerator<ConfigsType>::generate_required(const std::string& program_name) const {
    std::ostringstream help;
    help << colorize("Required Fields for ", ansi::BOLD, m_use_colors) << program_name << ":\n\n";
    
    std::ostringstream fields_out;
    auto fields = m_configs.get_fields();
    std::apply([&](auto&... field) {
        ((print_field_if_required(fields_out, field, "")), ...);
    }, fields);
    
    if (fields_out.str().empty()) {
        help << "No required fields found.\n";
    } else {
        help << fields_out.str();
    }
    
    return help.str();
}

template<typename ConfigsType>
template<typename T>
void HelpGenerator<ConfigsType>::print_group_structure(std::ostringstream&, const Config<T>&, size_t, const std::string&) const {
}

template<typename ConfigsType>
template<typename T>
void HelpGenerator<ConfigsType>::print_group_structure(std::ostringstream& out, const ConfigGroup<T>& group, size_t indent, const std::string& prefix) const {
    const std::string indent_str(indent * 2, ' ');
    const std::string full_prefix = prefix.empty() ? group.get_name() : prefix + "." + group.get_name();
    
    out << "  " << indent_str << colorize(group.get_name(), ansi::GREEN, m_use_colors) 
        << " " << colorize("(" + full_prefix + ")", ansi::GRAY, m_use_colors) << "\n";
    
    auto fields = group.get_fields();
    std::apply([&](auto&... field) {
        ((print_group_structure(out, field, indent + 1, full_prefix)), ...);
    }, fields);
}

template<typename ConfigsType>
std::string HelpGenerator<ConfigsType>::generate_groups(const std::string& program_name) const {
    std::ostringstream help;
    help << colorize("Configuration Groups for ", ansi::BOLD, m_use_colors) << program_name << ":\n\n";
    
    auto fields = m_configs.get_fields();
    std::apply([&](auto&... field) {
        ((print_group_structure(help, field, 0, "")), ...);
    }, fields);
    
    return help.str();
}

template<typename ConfigsType>
template<typename T>
bool HelpGenerator<ConfigsType>::print_field_if_matches(std::ostringstream&, const Config<T>&, const std::string&, const std::string&) const {
    return false;
}

template<typename ConfigsType>
template<typename T>
bool HelpGenerator<ConfigsType>::print_field_if_matches(std::ostringstream& out, const ConfigGroup<T>& group, const std::string& filter, const std::string& prefix) const {
    const std::string full_prefix = prefix.empty() ? group.get_name() : prefix + "." + group.get_name();
    
    if (group.get_name() == filter || full_prefix == filter) {
        print_field_hierarchical(out, group, 0, 80, prefix);
        return true;
    }
    
    bool found = false;
    auto fields = group.get_fields();
    std::apply([&](auto&... field) {
        ((found = print_field_if_matches(out, field, filter, full_prefix) || found), ...);
    }, fields);
    
    return found;
}

template<typename ConfigsType>
std::string HelpGenerator<ConfigsType>::generate_filtered(const std::string&, const std::string& group_filter) const {
    std::ostringstream help;
    help << colorize("Help for group '", ansi::BOLD, m_use_colors) << group_filter << "':\n\n";
    
    bool found = false;
    auto fields = m_configs.get_fields();
    std::apply([&](auto&... field) {
        ((found = print_field_if_matches(help, field, group_filter, "") || found), ...);
    }, fields);
    
    if (!found) {
        help << "No group found matching '" << group_filter << "'\n";
        help << "Use --help filters to see available groups\n";
    }
    
    return help.str();
}

template<typename ConfigsType>
template<typename T>
void HelpGenerator<ConfigsType>::collect_group_names_from_field(const Config<T>&, std::vector<std::string>&, const std::string&) const {
}

template<typename ConfigsType>
template<typename T>
void HelpGenerator<ConfigsType>::collect_group_names_from_field(const ConfigGroup<T>& group, std::vector<std::string>& names, const std::string& prefix) const {
    const std::string full_name = prefix.empty() ? group.get_name() : prefix + "." + group.get_name();
    names.push_back(full_name);
    
    auto fields = group.get_fields();
    std::apply([&](auto&... field) {
        ((collect_group_names_from_field(field, names, full_name)), ...);
    }, fields);
}

template<typename ConfigsType>
void HelpGenerator<ConfigsType>::collect_group_names(std::vector<std::string>& names, const std::string& prefix) const {
    auto fields = m_configs.get_fields();
    std::apply([&](auto&... field) {
        ((collect_group_names_from_field(field, names, prefix)), ...);
    }, fields);
}

template<typename ConfigsType>
std::string HelpGenerator<ConfigsType>::generate_filters(const std::string&) const {
    std::ostringstream help;
    help << colorize("Available Help Filters:", ansi::BOLD, m_use_colors) << "\n\n";
    help << colorize("  all", ansi::CYAN, m_use_colors) << "       - Show all configuration options\n";
    help << colorize("  groups", ansi::CYAN, m_use_colors) << "    - Show only the configuration group structure\n";
    help << colorize("  required", ansi::CYAN, m_use_colors) << "  - Show only required fields\n";
    
    std::vector<std::string> group_names;
    collect_group_names(group_names);
    
    if (!group_names.empty()) {
        help << "\n" << colorize("Available Groups:", ansi::BOLD, m_use_colors) << "\n";
        for (const auto& name : group_names) {
            help << colorize("  " + name, ansi::CYAN, m_use_colors) << "\n";
        }
    }
    
    return help.str();
}
